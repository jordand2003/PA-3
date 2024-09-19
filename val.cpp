#include "val.h"

Value Value ::operator+(const Value &op) const
{
    if (IsReal() || IsInt())
    {
        if (!op.IsReal() && !op.IsInt())
        {
            return Value();
        }
        else if (IsReal() && op.IsReal())
        {
            return Value(GetReal() + op.GetReal());
        }
        else if (IsReal() && op.IsInt())
        {
            return Value(GetReal() + (double)op.GetInt());
        }
        else if (IsInt() && op.IsReal())
        {
            return Value((double)GetInt() + op.GetReal());
        }
        else if (IsInt() && op.IsInt())
        {
            return Value(GetInt() + op.GetInt());
        }
    }

    return Value();
};

// numeric overloaded subtract op from this
Value Value::operator-(const Value &op) const
{
    if (IsReal() || IsInt())
    {
        if (!op.IsReal() && !op.IsInt())
        {
            return Value();
        }
        else if (IsReal() && op.IsReal())
        {
            return Value(GetReal() - op.GetReal());
        }
        else if (IsReal() && op.IsInt())
        {
            return Value(GetReal() - (double)op.GetInt());
        }
        else if (IsInt() && op.IsReal())
        {
            return Value((double)GetInt() - op.GetReal());
        }
        else if (IsInt() && op.IsInt())
        {
            return Value(GetInt() - op.GetInt());
        }
    }

    return Value();
};

// numeric overloaded multiply this by op
Value Value::operator*(const Value &op) const
{
    if (IsReal() || IsInt())
    {
        if (!op.IsReal() && !op.IsInt())
        {
            return Value();
        }
        else if (IsReal() && op.IsReal())
        {
            return Value(GetReal() * op.GetReal());
        }
        else if (IsReal() && op.IsInt())
        {
            return Value(GetReal() * (double)op.GetInt());
        }
        else if (IsInt() && op.IsReal())
        {
            return Value((double)GetInt() * op.GetReal());
        }
        else if (IsInt() && op.IsInt())
        {
            return Value(GetInt() * op.GetInt());
        }
    }

    return Value();
};

// numeric overloaded divide this by oper
Value Value::operator/(const Value &op) const
{
    if (IsReal() || IsInt())
    {
        if (!op.IsReal() && !op.IsInt())
        {
            return Value();
        }
        else if (IsReal() && op.IsReal())
        {
            return Value(GetReal() / op.GetReal());
        }
        else if (IsReal() && op.IsInt())
        {
            return Value(GetReal() / (double)op.GetInt());
        }
        else if (IsInt() && op.IsReal())
        {
            return Value((double)GetInt() / op.GetReal());
        }
        else if (IsInt() && op.IsInt())
        {
            return Value(GetInt() / op.GetInt());
        }
    }

    return Value();
};

// string concatenation of this with op
Value Value::Catenate(const Value &op) const
{
    if (IsString() && op.IsString())
    {
        return Value(GetString() + op.GetString());
    }
    else
    {
        return Value();
    }
};

// compute the value of this raised to the exponent op
Value Value::Power(const Value &op) const
{
    if ((IsInt() || IsReal()) && (op.IsInt() || op.IsReal()))
    {
        double base = IsInt() ? static_cast<double>(GetInt()) : GetReal();
        double exponent = op.IsInt() ? static_cast<double>(op.GetInt()) : op.GetReal();
        return Value(pow(base, exponent));
    }
    else
    {
        return Value();
    }
};

// overloaded equality operator of this with op
Value Value::operator==(const Value &op) const
{
    if ((IsInt() || IsReal()) && (op.IsInt() || op.IsReal()))
    {
        double leftValue = IsInt() ? static_cast<double>(GetInt()) : GetReal();
        double rightValue = op.IsInt() ? static_cast<double>(op.GetInt()) : op.GetReal();
        return Value(leftValue == rightValue);
    }
    else
    {
        return Value();
    }
};
// overloaded greater than operator of this with op
Value Value::operator>(const Value &op) const
{
    if ((IsInt() || IsReal()) && (op.IsInt() || op.IsReal()))
    {
        double leftValue = IsInt() ? static_cast<double>(GetInt()) : GetReal();
        double rightValue = op.IsInt() ? static_cast<double>(op.GetInt()) : op.GetReal();
        return Value(leftValue > rightValue);
    }
    else
    {
        return Value();
    }
};
// overloaded less than operator of this with op
Value Value::operator<(const Value &op) const
{
    if ((IsInt() || IsReal()) && (op.IsInt() || op.IsReal()))
    {
        double leftValue = IsInt() ? static_cast<double>(GetInt()) : GetReal();
        double rightValue = op.IsInt() ? static_cast<double>(op.GetInt()) : op.GetReal();
        return Value(leftValue < rightValue);
    }
    else
    {
        return Value();
    }
};
