#pragma once

//Probability may have value between 0 and 100
class Probability
{
public:
	Probability(void) : m_percent(0) {}
	Probability(int _percent) : m_percent(_percent) {}
	~Probability(void);

	int percent() const {
		return m_percent;
	}

private:
	int m_percent;
};

inline bool operator < (const Probability & first, const Probability & second)
{
	return first.percent() < second.percent();
}

//Alternate spelling
typedef Probability Possibility;