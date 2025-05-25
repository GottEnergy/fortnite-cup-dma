#pragma once

class CircularBuffer {
public:
    CircularBuffer(size_t size) : maxSize(size) {}

    void addValue(float value) {
        if (data.size() < maxSize) {
            data.push_back(value);
        }
        else {
            data[index] = value;
        }
        index = (index + 1) % maxSize;
    }

    const std::vector<float>& getValues() const { return data; }

private:
    size_t maxSize;
    size_t index = 0;
    std::vector<float> data;
};

namespace stats {


    inline CircularBuffer GDataData(1000);
    inline CircularBuffer updateCameraData(1000);
    inline CircularBuffer updatePlayerListData(1000);
    inline CircularBuffer updatePlayersData(1000);

    inline CircularBuffer memoryThreadData(1000);
    inline CircularBuffer mainThreadData(1000);

}