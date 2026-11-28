/*
 * ============================================================
 *         Reed-Solomon GF(2^8) Erasure Coding (RAID-6 / Cloud)
 * ============================================================
 *
 * Description:
 *   Systematic Reed-Solomon (k, m) erasure coding splits data into
 *   k equal-length data shards and computes m parity shards over
 *   the Galois Field GF(2^8) (modulo the AES/QR irreducible
 *   polynomial x^8 + x^4 + x^3 + x^2 + 1 = 0x11D).
 *
 *   If ANY subset of up to m shards (data, parity, or mixed) is
 *   lost due to disk failure or packet drop, all k original data
 *   shards are reconstructed losslessly:
 *     1. Build a (k + m) x k encoding matrix E whose top k x k
 *        block is the identity matrix I_k (so data shards stay
 *        unmodified) and whose bottom m x k block is a Cauchy
 *        matrix C_{i,j} = 1 / (x_i ^ y_j) in GF(2^8) (every square
 *        submatrix of a Cauchy matrix is guaranteed invertible!).
 *     2. Encode: Shards = E * Data.
 *     3. Decode from any k surviving rows of E: extract the k x k
 *        submatrix E_surv, invert E_surv over GF(2^8) via Gauss-
 *        Jordan elimination, and multiply: Data = E_surv^{-1} * Shards_surv.
 *
 * Complexity:
 *   GF(2^8) table setup: O(256)
 *   Encode m parity shards of L bytes: O(k * m * L)
 *   Reconstruct from k survivors:      O(k^3 + k^2 * L)
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <algorithm>

namespace reed_solomon {

    class GF256 {
        std::array<uint8_t, 512> exp_{};
        std::array<uint8_t, 256> log_{};

        GF256() {
            int x = 1;
            for (int i = 0; i < 255; ++i) {
                exp_[i] = static_cast<uint8_t>(x);
                log_[x] = static_cast<uint8_t>(i);
                x <<= 1;
                if (x & 0x100) x ^= 0x11D; // x^8 + x^4 + x^3 + x^2 + 1
            }
            for (int i = 255; i < 512; ++i) exp_[i] = exp_[i - 255];
        }

    public:
        static const GF256& instance() {
            static GF256 g;
            return g;
        }

        uint8_t mul(uint8_t a, uint8_t b) const {
            if (a == 0 || b == 0) return 0;
            return exp_[log_[a] + log_[b]];
        }

        uint8_t inv(uint8_t a) const {
            return exp_[255 - log_[a]];
        }

        uint8_t div(uint8_t a, uint8_t b) const {
            if (a == 0) return 0;
            return exp_[log_[a] + 255 - log_[b]];
        }
    };

    using Matrix = std::vector<std::vector<uint8_t>>;

    Matrix invert_matrix_gf256(Matrix A) {
        const auto& gf = GF256::instance();
        int n = static_cast<int>(A.size());
        Matrix inv(n, std::vector<uint8_t>(n, 0));
        for (int i = 0; i < n; ++i) inv[i][i] = 1;

        for (int col = 0; col < n; ++col) {
            int pivot = col;
            while (pivot < n && A[pivot][col] == 0) ++pivot;
            if (pivot == n) throw std::runtime_error("Singular matrix over GF(2^8)");
            std::swap(A[col], A[pivot]);
            std::swap(inv[col], inv[pivot]);

            uint8_t p_inv = gf.inv(A[col][col]);
            for (int j = 0; j < n; ++j) {
                A[col][j]   = gf.mul(A[col][j], p_inv);
                inv[col][j] = gf.mul(inv[col][j], p_inv);
            }
            for (int r = 0; r < n; ++r) {
                if (r == col || A[r][col] == 0) continue;
                uint8_t factor = A[r][col];
                for (int j = 0; j < n; ++j) {
                    A[r][j]   ^= gf.mul(factor, A[col][j]);
                    inv[r][j] ^= gf.mul(factor, inv[col][j]);
                }
            }
        }
        return inv;
    }

    class ReedSolomonCodec {
        int k_, m_;
        Matrix enc_; // (k + m) x k systematic Cauchy matrix

    public:
        ReedSolomonCodec(int k_data, int m_parity)
            : k_(k_data), m_(m_parity), enc_(k_data + m_parity, std::vector<uint8_t>(k_data, 0)) {
            const auto& gf = GF256::instance();
            for (int i = 0; i < k_; ++i) enc_[i][i] = 1;
            for (int r = 0; r < m_; ++r)
                for (int c = 0; c < k_; ++c)
                    enc_[k_ + r][c] = gf.inv(static_cast<uint8_t>((k_ + r) ^ c));
        }

        // Given k data shards of equal length, returns all k + m shards
        std::vector<std::vector<uint8_t>> encode(const std::vector<std::vector<uint8_t>>& data) const {
            const auto& gf = GF256::instance();
            size_t len = data[0].size();
            std::vector<std::vector<uint8_t>> shards(k_ + m_, std::vector<uint8_t>(len, 0));
            for (int i = 0; i < k_; ++i) shards[i] = data[i];

            for (int r = 0; r < m_; ++r) {
                auto& parity = shards[k_ + r];
                for (int c = 0; c < k_; ++c) {
                    uint8_t coeff = enc_[k_ + r][c];
                    const auto& src = data[c];
                    for (size_t b = 0; b < len; ++b)
                        parity[b] ^= gf.mul(coeff, src[b]);
                }
            }
            return shards;
        }

        // Reconstruct original k data shards given all shards and a boolean mask of which survived
        std::vector<std::vector<uint8_t>> reconstruct(const std::vector<std::vector<uint8_t>>& shards,
                                                      const std::vector<bool>& present) const {
            const auto& gf = GF256::instance();
            Matrix sub_mat(k_, std::vector<uint8_t>(k_));
            std::vector<int> surv_idx;
            for (int i = 0; i < k_ + m_ && static_cast<int>(surv_idx.size()) < k_; ++i) {
                if (present[i]) {
                    sub_mat[surv_idx.size()] = enc_[i];
                    surv_idx.push_back(i);
                }
            }
            Matrix dec = invert_matrix_gf256(sub_mat);
            size_t len = shards[surv_idx[0]].size();
            std::vector<std::vector<uint8_t>> recovered(k_, std::vector<uint8_t>(len, 0));

            for (int r = 0; r < k_; ++r) {
                for (int c = 0; c < k_; ++c) {
                    uint8_t coeff = dec[r][c];
                    if (coeff == 0) continue;
                    const auto& src = shards[surv_idx[c]];
                    for (size_t b = 0; b < len; ++b)
                        recovered[r][b] ^= gf.mul(coeff, src[b]);
                }
            }
            return recovered;
        }
    };

} // namespace reed_solomon

int main() {
    using namespace reed_solomon;
    std::cout << "============================================================\n";
    std::cout << "         Reed-Solomon GF(2^8) Erasure Coding (RAID-6 / Cloud)\n";
    std::cout << "============================================================\n\n";

    // RS(6, 3): 6 data shards + 3 parity shards (tolerates ANY 3 simultaneous shard erasures)
    const int K = 6, M = 3;
    const size_t SHARD_LEN = 256;
    ReedSolomonCodec rs(K, M);

    std::vector<std::vector<uint8_t>> data(K, std::vector<uint8_t>(SHARD_LEN));
    for (int i = 0; i < K; ++i)
        for (size_t b = 0; b < SHARD_LEN; ++b)
            data[i][b] = static_cast<uint8_t>((i * 73 + b * 31 + 19) & 0xFF);

    auto shards = rs.encode(data);

    // Exhaustively test EVERY possible combination of 3 erased shards out of 9 (C(9, 3) = 84 patterns)
    bool ok = true;
    int combinations = 0;
    for (int e1 = 0; e1 < K + M; ++e1)
        for (int e2 = e1 + 1; e2 < K + M; ++e2)
            for (int e3 = e2 + 1; e3 < K + M; ++e3) {
                ++combinations;
                std::vector<bool> present(K + M, true);
                present[e1] = present[e2] = present[e3] = false;
                auto damaged = shards;
                std::fill(damaged[e1].begin(), damaged[e1].end(), 0);
                std::fill(damaged[e2].begin(), damaged[e2].end(), 0);
                std::fill(damaged[e3].begin(), damaged[e3].end(), 0);

                auto rec = rs.reconstruct(damaged, present);
                if (rec != data) ok = false;
            }

    std::cout << "RS(6 data, 3 parity) over GF(2^8): tested all C(9, 3) = " << combinations
              << " 3-disk failure patterns.\n";
    std::cout << "100% lossless erasure recovery across all failure combinations: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
