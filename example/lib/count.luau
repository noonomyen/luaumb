local Counter = {}
Counter.__index = Counter

function Counter.new()
    local self = setmetatable({}, Counter)
    self.count = 0
    return self
end

type Counter = typeof(Counter.new())

function Counter:current(): number
    self = self :: Counter
    return self.count
end

function Counter:increment(amount: number?)
    self = self :: Counter
    self.count += amount or 1
end

return Counter
