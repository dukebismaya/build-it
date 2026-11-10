/**
 * @file chandy_lamport_distributed_snapshot.cpp
 * @brief Discrete-event simulation of the Chandy-Lamport Distributed Snapshot Algorithm in C++17.
 *
 * The Chandy-Lamport algorithm (1985) captures a consistent global state (snapshot)
 * of an asynchronous distributed system without pausing ongoing processing.
 *
 * System Model:
 *   - N processes communicating over directed, reliable, FIFO channels.
 *   - Process state: local variables (e.g. account balance).
 *   - Channel state: sequence of messages in transit.
 *
 * The Marker Rules:
 *   Marker-Sending Rule for Process P:
 *     1. P records its own internal state.
 *     2. Before sending any further application messages, P sends a Marker along
 *        every outgoing channel.
 *
 *   Marker-Receiving Rule for Process Q on incoming channel C:
 *     - If Q has not yet recorded its state:
 *         1. Q records its own internal state.
 *         2. Q records channel C as EMPTY in this snapshot.
 *         3. Q sends a Marker along each of its outgoing channels.
 *         4. Q begins recording in-flight messages on all incoming channels except C.
 *     - If Q has already recorded its state:
 *         1. Q stops recording messages on channel C.
 *         2. The recorded state of channel C consists of all messages received on C
 *            since Q recorded its state up to the receipt of this Marker.
 *
 * Global Consistency:
 *   Total money across all process states + all in-transit channel states equals the
 *   exact initial invariant total, proving the snapshot is globally consistent.
 *
 * Complexity:
 *   - Message overhead: O(|E|) markers where E is the number of directed channels.
 *   - Time: O(D) where D is network diameter.
 */

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <iomanip>
#include <numeric>
#include <map>

namespace distributed {

struct Message {
    enum Type { APPLICATION, MARKER };
    Type type;
    int senderId;
    int receiverId;
    int amount; // Payload for application messages
};

class Process {
public:
    int id;
    int balance;

    // Snapshot variables
    bool snapshotInitiated;
    int recordedBalance;
    std::map<int, bool> recordingChannel;           // channel from senderId -> isRecording
    std::map<int, std::vector<int>> recordedChannels; // channel from senderId -> in-transit amounts

    Process(int pId, int initBalance)
        : id(pId), balance(initBalance), snapshotInitiated(false), recordedBalance(-1) {}

    void initiateSnapshot(std::vector<Message>& outQueue, const std::vector<int>& outNeighbors) {
        snapshotInitiated = true;
        recordedBalance = balance;

        // Send markers to all outgoing channels
        for (int neighbor : outNeighbors) {
            outQueue.push_back({Message::MARKER, id, neighbor, 0});
        }
    }

    void handleMarker(int fromSender, std::vector<Message>& outQueue,
                      const std::vector<int>& inNeighbors, const std::vector<int>& outNeighbors) {
        if (!snapshotInitiated) {
            // First marker seen
            snapshotInitiated = true;
            recordedBalance = balance;
            recordedChannels[fromSender] = {}; // Channel fromSender is empty

            // Start recording all other incoming channels
            for (int inNode : inNeighbors) {
                if (inNode != fromSender) {
                    recordingChannel[inNode] = true;
                    recordedChannels[inNode] = {};
                }
            }

            // Propagate markers
            for (int outNode : outNeighbors) {
                outQueue.push_back({Message::MARKER, id, outNode, 0});
            }
        } else {
            // Subsequent marker on channel fromSender
            recordingChannel[fromSender] = false; // Stop recording this channel
        }
    }

    void handleApplicationMessage(const Message& msg) {
        balance += msg.amount;
        if (recordingChannel[msg.senderId]) {
            recordedChannels[msg.senderId].push_back(msg.amount);
        }
    }
};

class DistributedNetwork {
private:
    std::vector<Process> processes;
    std::vector<std::vector<int>> outAdj;
    std::vector<std::vector<int>> inAdj;
    std::queue<Message> messageQueue;

public:
    explicit DistributedNetwork(const std::vector<int>& initialBalances) {
        int n = static_cast<int>(initialBalances.size());
        outAdj.resize(n);
        inAdj.resize(n);
        for (int i = 0; i < n; ++i) {
            processes.emplace_back(i, initialBalances[i]);
        }
    }

    void addChannel(int from, int to) {
        outAdj[from].push_back(to);
        inAdj[to].push_back(from);
    }

    void transferMoney(int from, int to, int amount) {
        if (processes[from].balance >= amount) {
            processes[from].balance -= amount;
            messageQueue.push({Message::APPLICATION, from, to, amount});
        }
    }

    void startSnapshotAt(int initiator) {
        std::vector<Message> markers;
        // Start recording incoming channels for initiator
        for (int inNode : inAdj[initiator]) {
            processes[initiator].recordingChannel[inNode] = true;
            processes[initiator].recordedChannels[inNode] = {};
        }
        processes[initiator].initiateSnapshot(markers, outAdj[initiator]);
        for (const auto& m : markers) {
            messageQueue.push(m);
        }
    }

    void step() {
        if (messageQueue.empty()) return;
        Message msg = messageQueue.front();
        messageQueue.pop();

        int receiver = msg.receiverId;
        if (msg.type == Message::MARKER) {
            std::vector<Message> outMarkers;
            processes[receiver].handleMarker(msg.senderId, outMarkers, inAdj[receiver], outAdj[receiver]);
            for (const auto& m : outMarkers) {
                messageQueue.push(m);
            }
        } else {
            processes[receiver].handleApplicationMessage(msg);
        }
    }

    bool hasPendingMessages() const {
        return !messageQueue.empty();
    }

    void printSnapshotSummary() const {
        std::cout << "\n========================================\n";
        std::cout << "      RECORDED GLOBAL SNAPSHOT STATE     \n";
        std::cout << "========================================\n";

        int totalProcessMoney = 0;
        int totalInFlightMoney = 0;

        std::cout << "\n1. Local Process States:\n";
        for (const auto& p : processes) {
            std::cout << "  Process P" << p.id << ": Recorded Balance = $" << p.recordedBalance
                      << " (Current live = $" << p.balance << ")\n";
            totalProcessMoney += p.recordedBalance;
        }

        std::cout << "\n2. In-Flight Channel States:\n";
        for (const auto& p : processes) {
            for (const auto& ch : p.recordedChannels) {
                int sender = ch.first;
                int sumChannel = std::accumulate(ch.second.begin(), ch.second.end(), 0);
                totalInFlightMoney += sumChannel;

                std::cout << "  Channel (P" << sender << " -> P" << p.id << "): [ ";
                for (int val : ch.second) std::cout << "$" << val << " ";
                std::cout << "] Total = $" << sumChannel << "\n";
            }
        }

        int globalSnapshotTotal = totalProcessMoney + totalInFlightMoney;
        std::cout << "\nInvariant Verification:\n";
        std::cout << "  - Total in Process States : $" << totalProcessMoney << "\n";
        std::cout << "  - Total in Channel States : $" << totalInFlightMoney << "\n";
        std::cout << "  - Global Snapshot Sum     : $" << globalSnapshotTotal << "\n";
        std::cout << "  - Invariant Verified?       " << (globalSnapshotTotal == 1000 ? "YES ($1000 conserved)" : "MISMATCH") << "\n\n";
    }
};

} // namespace distributed

int main() {
    std::cout << "========================================\n";
    std::cout << "  CHANDY-LAMPORT DISTRIBUTED SNAPSHOT   \n";
    std::cout << "========================================\n\n";

    // 3 Processes with $1000 total: P0: $400, P1: $350, P2: $250
    std::vector<int> initialBalances = {400, 350, 250};
    distributed::DistributedNetwork net(initialBalances);

    // Form directed ring with bidirectional links
    net.addChannel(0, 1);
    net.addChannel(1, 0);
    net.addChannel(1, 2);
    net.addChannel(2, 1);
    net.addChannel(2, 0);
    net.addChannel(0, 2);

    std::cout << "System initialized with 3 processes. Total invariant money: $1000\n";
    std::cout << "  P0: $400, P1: $350, P2: $250\n\n";

    // P0 transfers $50 to P1
    std::cout << "Event 1: P0 transfers $50 to P1 (message in transit).\n";
    net.transferMoney(0, 1, 50);

    // P0 initiates snapshot while $50 is in transit to P1!
    std::cout << "Event 2: P0 initiates global snapshot.\n";
    net.startSnapshotAt(0);

    // P1 transfers $30 to P2
    std::cout << "Event 3: P1 transfers $30 to P2.\n";
    net.transferMoney(1, 2, 30);

    // Run distributed simulation to completion
    std::cout << "Processing asynchronous event loop...\n";
    while (net.hasPendingMessages()) {
        net.step();
    }

    net.printSnapshotSummary();
    std::cout << "Program finished successfully.\n";
    return 0;
}
