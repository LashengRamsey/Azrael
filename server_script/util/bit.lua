module('bit', package.seeall)

local function __andBit(left,right)
    return (left == 1 and right == 1) and 1 or 0    
end

local function __orBit(left, right)
    return (left == 1 or right == 1) and 1 or 0
end

local function __xorBit(left, right)
    return (left + right) == 1 and 1 or 0
end

local function __base(left, right, op)
    if left < right then
        left, right = right, left
    end
    local res = 0
    local shift = 1
    while left ~= 0 do
        local ra = left % 2
        local rb = right % 2
        res = shift * op(ra,rb) + res
        shift = shift * 2
        left = math.modf( left / 2)
        right = math.modf( right / 2)
    end
    return res
end

function andOp(left, right)--按位与
    return __base(left, right, __andBit)
end

function xorOp(left, right)--按位或
    return __base(left, right, __xorBit)
end

function orOp(left, right)--异或
    return __base(left, right, __orBit)
end

function notOp(left)--非
    return left > 0 and -(left + 1) or -left - 1 
end

function lShiftOp(left, num)--左移
    return left * (2 ^ num)
end

function rShiftOp(left,num)--右移
    return math.floor(left / (2 ^ num))
end



