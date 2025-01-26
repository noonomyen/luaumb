local print_counter = require("./lib/print")
local counter = require("./lib/count")
local counter_b = require("./b").counter_b
local counter_c = require("./c").counter_c

local counter_a = counter.new()

print("----- A (A+1)")

counter_a:increment()

print_counter("Counter A", counter_a)
print_counter("Counter B", counter_b)
print_counter("Counter C", counter_c)

return {
    counter_a = counter_a,
    counter_b = counter_b,
    counter_c = counter_c
}
