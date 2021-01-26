HX711Multi
=====

An Arduino library to interface the Avia Semiconductor HX711 24-Bit Analog-to-Digital Converter (ADC) for Weight Scales.

This is my humble attempt at creating an Arduino library for this ADC:
http://www.dfrobot.com/image/data/SEN0160/hx711_english.pdf

Other libraries exist, including this very good one, which I first used and which is the starting point for my library:
https://github.com/bogde/HX711

Although other libraries exist, I needed a slightly different approach, so here's how my library is different than others:

1. Not waiting for a convesion, but instead implement an update function, that returns the channe converted, but most of the
time, just returning -1 meaning that there are no new value to use.

2. Implementing an array as a buffer, where converted values are stored. This gives the possibility of making running averages
that can contain the latest conversions - the number of values possible in the buffer can be given to the constructor.

3. Letting the update handle two loadcells in the samme object, where the conversion switches automatically between the two loadcells
This has the unfortunate sideeffect, that the conversion takes about 5 times longer, giving about 10 times lower conversion rate 
at each cell. The library can handle different offeset ad different scaling at the two loadcells