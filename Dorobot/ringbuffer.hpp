#pragma once

template<typename T, int size>
class RingBuffer
{
public:
	RingBuffer()
	{
	}

	void add(const T& element)
	{
		if (this->buffer.size() == size) {
			this->buffer[tail] = element;
			this->tail++;
			if (this->tail == this->buffer.size()) this->tail = 0;
		}
		else {
			this->buffer.push_back(element);
		}
	}

	T get(int i)
	{
		if (i > this->buffer.size() - 1)
			throw std::out_of_range("Argument out of range");

		int pos = tail + i;
		pos %= this->buffer.size();
		return this->buffer[pos];
	}

	int getSize()
	{
		return this->buffer.size();
	}

	std::vector<T> readAll()
	{
		std::vector<T> vec = std::vector<T>(this->buffer.begin() + this->tail, this->buffer.end());
		std::vector<T> vec2 = std::vector<T>(this->buffer.begin(), this->buffer.begin() + this->tail);

		std::move(vec2.begin(), vec2.end(), std::back_inserter(vec));

		return vec;
	}

	void clear()
	{
		this->buffer.clear();
		tail = 0;
	}

	void trim(int x, int y)
	{
		std::vector<T> copy = readAll();
		this->clear();

		for (int i = x; i <= y; i++) {
			this->add(copy[i]);
		}
	}

private:
	std::vector<T> buffer;
	int tail = 0;

};
