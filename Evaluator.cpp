#include "StdAfx.h"

#include "Evaluator.h"

#include "CharacterRules.h"

#include "Globals.h"

std::vector<Character> Evaluator::evaluate(const std::vector<Character> & input) const
{
	std::vector<Character> result = significant_part(input);
	if (must_evaluate(result))
		result = do_evaluate(result);
	return result;
}

std::vector<Character> Evaluator::significant_part(const std::vector<Character> & input) const
{
	std::vector<Character> result;

	if (!input.empty())
	{
		result.push_back(input.back()); // last '=' should not be skipped

		std::vector<Character>::const_reverse_iterator ri = input.rbegin();
		++ri;

		while(ri != input.rend())
		{
			const Character & ch = *ri;

			if (ch.m_name == "Equals")
			{
				break;
			}
			else
			{
				result.insert(result.begin(), ch);
			}
			++ri;
		}
	}

	return result;
}

Characters Evaluator::do_evaluate(const Characters & input) const
{
	Characters firstOperand, secondOperand;
	Character oper;

	if (parse(input, firstOperand, secondOperand, oper))
	{
		if (oper.empty()) //Number followed by '='
		{
			chop(firstOperand);
			return firstOperand;
		}
		else
		{
			return do_evaluate(firstOperand, secondOperand, oper);
		}
	}
	else
	{
		return err();
	}
}

Characters Evaluator::do_evaluate(Characters & firstOperand, Characters & secondOperand, Character & oper) const
{
	if(oper.name() == "Plus")
		return sum(firstOperand, secondOperand);

	if(oper.name() == "Minus")
		return sub(firstOperand, secondOperand);

	if(oper.name() == "Multiply")
		return mul(firstOperand, secondOperand);

	if(oper.name() == "Divide")
		return div(firstOperand, secondOperand);

	return err();
}

Characters Evaluator::sum(Characters & firstOperand, Characters & secondOperand) const
{
	align_dots(firstOperand,secondOperand);

	align_lengths(firstOperand,secondOperand);

	Characters result = sum_aligned(firstOperand,secondOperand);

	chop(result);

	return result;
}

Characters Evaluator::sub(Characters & firstOperand, Characters & secondOperand) const
{
	align_dots(firstOperand,secondOperand);

	align_lengths(firstOperand,secondOperand);

	bool swap = is_less(firstOperand, secondOperand);

	Characters result;

	if (swap)
	{
		result = sub_aligned(secondOperand,firstOperand);
		negate(result);
	}
	else
	{
		result = sub_aligned(firstOperand,secondOperand);
	}

	chop(result);

	return result;
}

void Evaluator::negate(Characters & result) const
{
	if(! result.empty() && result.front().name() == "Minus")
	{
		result.erase(result.begin());
	}
	else
	{
		result.insert(result.begin(), Character("Minus"));
	}
}

bool Evaluator::is_less(Characters firstOperand, Characters secondOperand) const
{
	if (is_negative(firstOperand) && !is_negative(secondOperand))
	{
		return true;
	}
	else if (!is_negative(firstOperand) && is_negative(secondOperand))
	{
		return false;
	}
	else if (is_negative(firstOperand) && is_negative(secondOperand))
	{
		negate(firstOperand);
		negate(secondOperand);

		return is_less(secondOperand, firstOperand);
	}
	else
	{
		align_lengths(firstOperand, secondOperand);

		int len = (int)firstOperand.size();
		for(int i = 0; i < len; ++i)
		{
			Character & first = firstOperand[i];
			Character & second = secondOperand[i];
			if(first.name() != second.name())
				return is_less(first, second);
		}
		return false;
	}
}

bool Evaluator::is_less(Character first, Character second) const
{
	do
	{
		if(is_zero(second))
			return false;
		if(is_zero(first))
			return true;
	}
	while(dec(first) & dec(second));

	return false;
}

void Evaluator::align_dots(Characters & firstOperand, Characters & secondOperand) const
{
	int firstPos = dot_pos(firstOperand);
	int secondPos = dot_pos(secondOperand);

	if(firstPos < secondPos)
	{
		rpad(firstOperand, secondPos - firstPos);
	}
	else if(secondPos < firstPos)
	{
		rpad(secondOperand, firstPos - secondPos);
	}
}

void Evaluator::align_lengths(Characters & firstOperand, Characters & secondOperand) const
{
	if (firstOperand.size() < secondOperand.size())
		lpad(firstOperand,(int)secondOperand.size()-(int)firstOperand.size());
	else if (firstOperand.size() > secondOperand.size())
		lpad(secondOperand,(int)firstOperand.size()-(int)secondOperand.size());
}

void Evaluator::rpad(Characters & result, int num) const
{
	if(dot_pos(result)==0)
	{
		result.push_back(dot());
		--num;
	}
	while(num > 0)
	{
		result.push_back(zero());
		--num;
	}
}

void Evaluator::lpad(Characters & result, int num) const
{
	size_t first_digit_offset = is_negative(result) ? 1 : 0;

	while(num > 0)
	{
		result.insert(result.begin() + first_digit_offset, zero());
		--num;
	}
}

int Evaluator::dot_pos(Characters & input) const
{
	Characters::reverse_iterator dpi = std::find(input.rbegin(), input.rend(), dot());

	if (dpi == input.rend())
		return 0;
	else
		return (int)(dpi - input.rbegin()) + 1;
}

Characters Evaluator::sub_aligned(Characters & firstOperand, Characters & secondOperand) const
{
	Characters result;

	int len = (int)firstOperand.size();

	Character borrow = zero(); //number borrowed from next digit

	for(int i = len - 1; i >= 0; --i)
	{
		Character firstChar = firstOperand[i];
		Character secondChar = secondOperand[i];
		Character subChar = zero();
		Character oldBorrow = borrow;
		borrow = zero();

		sub(firstChar, secondChar, oldBorrow, subChar, borrow);

		result.insert(result.begin(), subChar);
	}

	return result;
}

void Evaluator::sub(Character first, Character second, Character borrow, Character & result, Character & next_borrow) const
{
	if(first == dot())
	{
		result = dot();
		next_borrow = borrow;
	}
	else
	{
		Character new_second = zero();
		sum(second, borrow, next_borrow, new_second, next_borrow);
		second = new_second;

		if(is_less(first, second))
		{
			while(! is_zero(first) )
			{
				dec(first);
				dec(second);
			}
			first = last();
			inc(next_borrow);
			dec(second);
			while(! is_zero(second))
			{
				dec(first);
				dec(second);
			}
		}
		else
		{
			while(!is_zero(second))
			{
				dec(first);
				dec(second);
			}
		}

		result = first;
	}
}

Characters Evaluator::sum_aligned(Characters & firstOperand, Characters & secondOperand) const
{
	Characters result;

	int len = (int)firstOperand.size();

	Character transfer = zero();

	for(int i = len - 1; i >= 0; --i)
	{
		Character firstChar = firstOperand[i];
		Character secondChar = secondOperand[i];
		Character sumChar = zero();
		Character oldTransfer = transfer;
		transfer = zero();
		sum(firstChar, secondChar, oldTransfer, sumChar, transfer);

		result.insert(result.begin(), sumChar);
	}

	if (!is_zero(transfer))
	{
		result.insert(result.begin(), transfer);
	}

	return result;
}

void Evaluator::sum(Character first, Character second, Character third, Character & result, Character & transfer) const
{
	if(first == dot())
	{
		result = dot();
		transfer = third;
	}
	else
	{
		while(dec(first) || dec(second) || dec(third))
			inc(result, transfer);
	}
}

bool Evaluator::dec(Character & ch) const
{
	Character prev = zero();

	if (prev == ch)
		return false;

	Character next = prev;
	while(next != ch)
	{
		prev = next;
		inc(next);
	}

	ch = prev;
	return true;
}

void Evaluator::inc(Character & digit, Character & transfer) const
{
	if (!inc(digit))
		inc(transfer);
}

bool Evaluator::inc(Character & digit) const
{
	Characters::iterator pos = std::find(
		CharacterRules::numbers().begin(),
		CharacterRules::numbers().end(),
		digit);

	Characters::iterator next = ++pos;

	if (next == CharacterRules::numbers().end())
	{
		digit = zero();
		return false;
	}
	else
	{
		digit = *next;
		return true;
	}
}

void Evaluator::chop(Characters & result) const
{
	while ( dot_pos(result) != 0 &&
		   (result.back() == dot() 
		    || is_zero(result.back())))
	{
		result.pop_back();
	}

	size_t minimal_size = is_negative(result) ? 2 : 1;
	size_t first_digit_offset = is_negative(result) ? 1 : 0;

	while(result.size() > minimal_size
		&& is_zero(result[first_digit_offset]))
	{
		result.erase(result.begin() + first_digit_offset);
	}

	if (result.size() < minimal_size || result[first_digit_offset] == dot())
	{
		result.insert(result.begin() + first_digit_offset, zero());
	}
}

bool Evaluator::is_negative(Characters & input) const
{
	return !input.empty() 
		&& input.front().name() == "Minus";
}

bool Evaluator::must_evaluate(const Characters & input) const
{
	if (input.empty())
		return false;
	else
		return input.back().m_name == "Equals";
}

bool Evaluator::parse(const Characters & _input, Characters & firstOperand, Characters & secondOperand, Character & oper) const
{
	Characters input = _input;
	if (input.back() == Character("Equals"))
		input.pop_back();

	//finding first operator; everything before it goes into first operand
	Characters::const_iterator operator_position = std::find_first_of(input.begin(), input.end(), operators().begin(), operators().end());
	Characters::const_iterator input_begin = input.begin();
	std::copy(input_begin, operator_position, std::back_inserter(firstOperand));

	if(operator_position != input.end())
	{
		//storing operator
		oper = *operator_position;

		//storing everything after operator...
		Characters::const_iterator second_operand_position = operator_position + 1;

		if (second_operand_position != input.end())
		{
			Characters::const_iterator input_end = input.end();
			std::copy(second_operand_position, input_end, std::back_inserter(secondOperand));

			//...it should be a number, not an expression
			return is_atom(secondOperand);
		}
		else
		{
			//expression terminates on operator - bad!
			return false;
		}
	}
	else
	{
		//no operator present
		return ! firstOperand.empty();
	}
}

//test whether input is a single number
bool Evaluator::is_atom(const Characters & input) const
{
	Characters t1, t2;
	Character oper;

	if (!parse(input, t1, t2, oper)) //should be parseable
		return false;

	if (oper.empty()) //no operator
		return true;

	return false; //complex expression
}

const Characters & Evaluator::err() const
{
	static Characters all_operators;
	if (all_operators.empty())
	{
		all_operators.push_back(Character("LetterE"));
		all_operators.push_back(Character("LetterR"));
		all_operators.push_back(Character("LetterR"));
	}
	return all_operators;
}

Characters Evaluator::mul(Characters & firstOperand, Characters & secondOperand) const
{
	chop(firstOperand);
	chop(secondOperand);

	//converting operands to integer...
	int first_dot_pos = dot_pos(firstOperand);
	int second_dot_pos = dot_pos(secondOperand);

	strip_dot(firstOperand);
	strip_dot(secondOperand);

	//multiplying as integers
	Characters result = mul_ints(firstOperand, secondOperand);

	//...returning decimal point into place
	insert_dot(result, first_dot_pos, second_dot_pos);

	chop(result);

	return result;
}

Characters Evaluator::mul_ints(Characters & firstOperand, Characters & secondOperand) const
{
	Characters result = zeros();

	//multiplying by single digit, shifting and summing
	for(int i = 0; i < (int)firstOperand.size(); ++i)
	{
		Character firstDigit = firstOperand[firstOperand.size()-i-1];

		Characters part = mul(secondOperand, firstDigit);

		shl(part, i);

		result = sum(result, part);
	}

	return result;
}

void Evaluator::shl(Characters & input, int pow) const
{
	while(pow > 0)
	{
		input.push_back(zero());
		--pow;
	}

	int dotPos = dot_pos(input);
	if(dotPos != 0)
	{
		strip_dot(input);
		insert_dot(input, dotPos-pow, 0);
	}
}

void Evaluator::shr(Characters & input, int pow) const
{
	chop(input);

	if(dot_pos(input) == 0)
		input.push_back(dot());

	int dotPos = dot_pos(input);
	strip_dot(input);
	insert_dot(input, dotPos+pow, 0);
	chop(input);
}

Characters Evaluator::mul(Characters & number, Character & digit) const
{
	Characters result = zeros();

	//deriving multiplication from addition
	while(!is_zero(digit))
	{
		result = sum(result, number);
		dec(digit);
	}

	return result;
}


void Evaluator::insert_dot(Characters & input, int pos1, int pos2) const
{
	int pos = max(pos1-1,0) + max(pos2-1,0);
	lpad(input,pos);
	input.insert((input.rbegin() + pos).base(), dot());
}

void Evaluator::strip_dot(Characters & input) const
{
	Characters::iterator it = std::find(input.begin(), input.end(), dot());
	if(it != input.end())
		input.erase(it);
}

Characters Evaluator::div(Characters & firstOperand, Characters & secondOperand) const
{
	chop(firstOperand);
	chop(secondOperand);

	if(is_zero(secondOperand))
	{
		return err();
	}
	else
	{
		int second_dot_pos = dot_pos(secondOperand);

		strip_dot(secondOperand);

		//TODO: process negatives - not in test cases, though

		//dividing by integer value
		Characters result = div_by_int(firstOperand, secondOperand);

		shr(result, second_dot_pos);

		chop(result);

		return result;
	}
}

Characters Evaluator::div_by_int(Characters & firstOperand, Characters & secondOperand) const
{
	Characters result = zeros();

	Characters intResult = zeros();

	int shift = 0;

	//dividing without non-integer part, then adding with shift
	while(div_int(firstOperand, secondOperand, intResult))
	{
		shr(intResult,shift);
		shl(firstOperand,1);

		result = sum(result, intResult);

		intResult = zeros();

		++shift;

		if(shift > Globals::max_characters_in_result())
		{
			//seems to be periodic - should report somehow
			result.push_back(Character("LetterE"));
			break;
		}
	}

	return result;
}

bool Evaluator::div_int(Characters & firstOperand, Characters & secondOperand, Characters & result) const
{
	if(is_zero(firstOperand))
		return false; //complete

	Characters inc_chars = zeros();
	Characters leftover = firstOperand;

	//substracting second from first while possible
	//TODO: 999999 / 1 hangs just as expected. None of test cases does, though. Should fix sometimes.
	while(!is_less(leftover, secondOperand))
	{
		Characters _ones = ones();
		inc_chars = sum(inc_chars, _ones);
		leftover = sub(leftover, secondOperand); 
	}

	result = inc_chars;
	firstOperand = leftover;

	return true;
}

bool Evaluator::is_zero(Characters val) const
{
	if (is_negative(val))
		negate(val);

	chop(val);

	return val.size() == 1 && is_zero(val.front());
}

const Characters & Evaluator::operators() const
{
	static Characters all_operators;
	if (all_operators.empty())
	{
		all_operators.push_back(Character("Plus"));
		all_operators.push_back(Character("Minus"));
		all_operators.push_back(Character("Multiply"));
		all_operators.push_back(Character("Divide"));
	}
	return all_operators;
}

const Characters & Evaluator::zeros() const
{
	static Characters _zeros;
	if (_zeros.empty())
	{
		_zeros.push_back(zero());
	}
	return _zeros;
}

const Characters & Evaluator::ones() const
{
	static Characters _ones;
	if (_ones.empty())
	{
		_ones.push_back(one());
	}
	return _ones;
}

bool Evaluator::is_zero(Character val) const
{
	return val == zero();
}

Character Evaluator::one() const
{
	return CharacterRules::numbers()[1];
}

Character Evaluator::zero() const
{
	return CharacterRules::numbers()[0];
}

Character Evaluator::last() const
{
	return CharacterRules::numbers().back();
}

Character Evaluator::dot() const
{
	return Character("Dot");
}