local c = require "dh64.c"
local priv1, pub1 = c.pair()
print(priv1, pub1)
local priv2, pub2 = c.pair()
print(priv2, pub2)
local s1 = c.secret(priv1, pub2)
local s2 = c.secret(priv2, pub1)
print(s1, s2)
