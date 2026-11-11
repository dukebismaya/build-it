/**
 * @file zobrist_hashing_game_board.cpp
 * @brief Zobrist Hashing for incremental O(1) board state hashing & transposition tables in C++17.
 *
 * Overview (Albert Zobrist, 1970):
 *   Zobrist Hashing converts a board game state into a uniform 64-bit integer key.
 *   It is the standard hashing technique used in Chess, Go, and Shogi engines.
 *
 * Key Properties:
 *   1. Incremental O(1) Updates: Instead of scanning the whole board after a move,
 *      the hash is updated in O(1) time via XOR:
 *        hash ^= table[oldSquare][piece]
 *        hash ^= table[newSquare][piece]
 *        hash ^= sideToMoveKey
 *   2. Transposition Invariance: Because bitwise XOR is commutative and associative,
 *      reaching the same board state through different move orders yields the
 *      exact same 64-bit Zobrist key.
 *   3. Reversibility: Unmaking a move applies the exact same XOR operations.
 *
 * Complexity:
 *   - Table Initialization: O(Squares * PieceTypes)
 *   - Move/Unmake Hash Update: O(1)
 *   - Full Board Hash Verification: O(Squares)
 */

#include <iostream>
#include <vector>
#include <array>
#include <random>
#include <cstdint>
#include <unordered_map>
#include <iomanip>
#include <string>

namespace game_ai {

enum Piece {
    EMPTY = 0,
    WHITE_PAWN = 1,
    WHITE_KNIGHT = 2,
    BLACK_PAWN = 3,
    BLACK_KNIGHT = 4,
    NUM_PIECE_TYPES = 5
};

class ZobristBoard {
private:
    static constexpr int BOARD_SIZE = 16; // 4x4 board
    std::array<Piece, BOARD_SIZE> squares;
    bool whiteToMove;

    // Zobrist keys: [square][pieceType]
    std::array<std::array<uint64_t, NUM_PIECE_TYPES>, BOARD_SIZE> pieceKeys;
    uint64_t sideKey;
    uint64_t currentHash;

    void initKeys(uint64_t seed) {
        std::mt19937_64 rng(seed);
        for (int sq = 0; sq < BOARD_SIZE; ++sq) {
            pieceKeys[sq][EMPTY] = 0ULL;
            for (int p = 1; p < NUM_PIECE_TYPES; ++p) {
                pieceKeys[sq][p] = rng();
            }
        }
        sideKey = rng();
    }

public:
    explicit ZobristBoard(uint64_t seed = 0xDEADBEEFULL)
        : whiteToMove(true), currentHash(0ULL) {
        squares.fill(EMPTY);
        initKeys(seed);
    }

    void setPiece(int sq, Piece p) {
        if (squares[sq] != EMPTY) {
            currentHash ^= pieceKeys[sq][squares[sq]];
        }
        squares[sq] = p;
        if (p != EMPTY) {
            currentHash ^= pieceKeys[sq][p];
        }
    }

    /**
     * @brief Performs a move incrementally in O(1) time.
     * @return Captured piece (for unmake).
     */
    Piece makeMove(int fromSq, int toSq) {
        Piece movingPiece = squares[fromSq];
        Piece captured = squares[toSq];

        // Remove moving piece from old square
        currentHash ^= pieceKeys[fromSq][movingPiece];
        squares[fromSq] = EMPTY;

        // If target occupied, remove captured piece from hash
        if (captured != EMPTY) {
            currentHash ^= pieceKeys[toSq][captured];
        }

        // Place moving piece on target square
        squares[toSq] = movingPiece;
        currentHash ^= pieceKeys[toSq][movingPiece];

        // Toggle side to move
        whiteToMove = !whiteToMove;
        currentHash ^= sideKey;

        return captured;
    }

    void unmakeMove(int fromSq, int toSq, Piece captured) {
        Piece movingPiece = squares[toSq];

        // Toggle side back
        whiteToMove = !whiteToMove;
        currentHash ^= sideKey;

        // Remove moving piece from target
        currentHash ^= pieceKeys[toSq][movingPiece];
        squares[toSq] = captured;

        // Restore captured piece if any
        if (captured != EMPTY) {
            currentHash ^= pieceKeys[toSq][captured];
        }

        // Restore moving piece to origin
        squares[fromSq] = movingPiece;
        currentHash ^= pieceKeys[fromSq][movingPiece];
    }

    /**
     * @brief Recomputes hash from scratch in O(BOARD_SIZE) to verify incremental accuracy.
     */
    uint64_t computeFullHash() const {
        uint64_t h = 0ULL;
        for (int sq = 0; sq < BOARD_SIZE; ++sq) {
            if (squares[sq] != EMPTY) {
                h ^= pieceKeys[sq][squares[sq]];
            }
        }
        if (!whiteToMove) {
            h ^= sideKey;
        }
        return h;
    }

    uint64_t getHash() const { return currentHash; }
};

} // namespace game_ai

int main() {
    std::cout << "========================================\n";
    std::cout << "      ZOBRIST HASHING GAME BOARD        \n";
    std::cout << "   (Incremental O(1) Transpositions)    \n";
    std::cout << "========================================\n\n";

    game_ai::ZobristBoard boardA(123456789ULL);
    game_ai::ZobristBoard boardB(123456789ULL);

    // Setup identical starting positions:
    // White Knight at sq 0, White Pawn at sq 2
    // Black Knight at sq 15, Black Pawn at sq 13
    auto setupBoard = [](game_ai::ZobristBoard& b) {
        b.setPiece(0, game_ai::WHITE_KNIGHT);
        b.setPiece(2, game_ai::WHITE_PAWN);
        b.setPiece(15, game_ai::BLACK_KNIGHT);
        b.setPiece(13, game_ai::BLACK_PAWN);
    };

    setupBoard(boardA);
    setupBoard(boardB);

    std::cout << std::hex << std::uppercase;
    std::cout << "Initial Hash Board A: 0x" << std::setw(16) << std::setfill('0') << boardA.getHash() << "\n";
    std::cout << "Initial Hash Board B: 0x" << std::setw(16) << std::setfill('0') << boardB.getHash() << "\n\n";

    // Sequence 1 on Board A: White Knight 0->5, Black Knight 15->10, White Pawn 2->6
    boardA.makeMove(0, 5);
    boardA.makeMove(15, 10);
    boardA.makeMove(2, 6);

    // Sequence 2 on Board B (Transposition! Different move order to same position):
    // White Pawn 2->6, Black Knight 15->10, White Knight 0->5
    boardB.makeMove(2, 6);
    boardB.makeMove(15, 10);
    boardB.makeMove(0, 5);

    std::cout << "After Transposition Move Sequences:\n";
    std::cout << "  Sequence 1 (Board A) Hash : 0x" << std::setw(16) << std::setfill('0') << boardA.getHash() << "\n";
    std::cout << "  Sequence 2 (Board B) Hash : 0x" << std::setw(16) << std::setfill('0') << boardB.getHash() << "\n";
    std::cout << "  Full Recompute Check A    : 0x" << std::setw(16) << std::setfill('0') << boardA.computeFullHash() << "\n\n";

    std::cout << std::dec << std::setfill(' ');
    std::cout << "Verification:\n";
    std::cout << "  - Transposition Invariance (Hash A == Hash B)? "
              << (boardA.getHash() == boardB.getHash() ? "PASS" : "FAIL") << "\n";
    std::cout << "  - Incremental vs Full Hash Match?              "
              << (boardA.getHash() == boardA.computeFullHash() ? "PASS" : "FAIL") << "\n";

    // Unmake moves on Board A
    boardA.unmakeMove(2, 6, game_ai::EMPTY);
    boardA.unmakeMove(15, 10, game_ai::EMPTY);
    boardA.unmakeMove(0, 5, game_ai::EMPTY);

    game_ai::ZobristBoard initialRef(123456789ULL);
    setupBoard(initialRef);
    std::cout << "  - Unmake Reversibility (Restored Initial)?     "
              << (boardA.getHash() == initialRef.getHash() ? "PASS" : "FAIL") << "\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
