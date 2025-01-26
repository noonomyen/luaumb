local print_counter = require("./lib/print")
local counter = require("./lib/count")

local counter_c = counter.new()

print("----- C (C+15)")

counter_c:increment(15)
print_counter("Counter C", counter_c)

return {
    counter_c = counter_c
}
