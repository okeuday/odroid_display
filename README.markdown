Odroid-C1 Display Service for [CloudI](http://cloudi.org)
=========================================================

Currently only the [16x2 LCD display](http://www.hardkernel.com/main/products/prdt_info.php?g_code=G141637550797) is supported.
This example shows how CloudI can isolate C dependencies for hardware so
higher-level data processing can run decoupled (keeping the fault-tolerance
constraints on both pieces of source code separate).

Usage
-----

The format for the service request is:

    uint8  (7 status LEDs bitmask to set as off)
    uint8  (7 status LEDs bitmask to set as on)
    uint8  (7 status LEDs bitmask to set opposite value (toggle))
    char[] (32 UTF8 characters, '\0' causes no display change)

The characters supported are limited by the HD44780U A00 character set
which is commonly used for LCD displays.

The supported UTF8 characters are:

* ' ' (no-break space)
* ¢
* ¥
* °
* µ
* ·
* ä
* ñ
* ö
* ÷
* ü
* Σ (upper greek sigma)
* Ω (upper greek omega)
* α (lower greek alpha)
* β (lower greek beta)
* γ (lower greek delta)
* ε (lower greek epsilon)
* θ (lower greek theta)
* μ (lower greek mu)
* π (lower greek pi)
* ρ (lower greek rho)
* ₤ (lira currency)
* ← (left arrow)
* → (right arrow)
* ∑ (n-ary summation (upper greek sigma))
* √ (square root)
* ∞ (infinity)
* ⌜ (top left corner)
* ⌟ (bottom right corner)
* █ (full block)

The unsupported ASCII characters are:

* \
* ~

TODO
----

* Add UTF8 Japanese characters
* Add support for the A02 character set (?)

