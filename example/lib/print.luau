type Counter = typeof(require("./count").new())

function print_counter(name: string, counter: Counter)
    print(name .. " [" .. tostring(counter) .. "]: " .. counter:current())
end

print("LIB/PRINT LOADED")

return print_counter
