local print_counter = require("./lib/print")
local counters = require("./a")
local hello = require("./hello").hello

print("----- MAIN")

print_counter("Counter A", counters.counter_a)
print_counter("Counter B", counters.counter_b)
print_counter("Counter C", counters.counter_c)

--

print("-----")

local pi = require("./lib/pi").value
print("PI: " .. pi)

print("-----")

print("Try to print Counter without require: " .. tostring(Counter))

hello()

print("MAIN LOADED")
