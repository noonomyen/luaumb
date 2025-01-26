local print_counter = require("./lib/print")
local counter = require("./lib/count")
local counter_c = require("./c").counter_c

local counter_b = counter.new()

print("----- B (B+20) (C+10)")

counter_b:increment(20)
counter_c:increment(10)

print_counter("Counter B", counter_b)
print_counter("Counter C", counter_c)

return {
    counter_b = counter_b,
    counter_c = counter_c
}
