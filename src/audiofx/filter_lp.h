/**
 * References:
 *
 * Resonant filter algorithm:
 * https://www.musicdsp.org/en/latest/Filters/29-resonant-filter.html
 * https://github.com/bdejong/musicdsp/blob/master/source/Filters/29-resonant-filter.rst
 *
 * Replacing mults with shifts: (n*x) -> (n>>a + n>>b)
 * https://people.ece.cornell.edu/land/courses/ece4760/Math/DigitalFiltersVersion2.pdf
 *
 * Low-level fixed point ASM implementation:
 * https://github.com/MeeBlip/meeblip-synth/blob/master/meeblip-se.asm
 *
 */