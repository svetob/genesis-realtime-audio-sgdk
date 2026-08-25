# UI Layout

```

                      PCM STREAM


   |      ....                 ..... .                 |
   |   ...    ....            .     . .    .           |
   | ..           ....      ..         .... ...      ..|
   |.                 ......                   ......  |



 (A) < ..EAM    FILTER  [  ECHO  ]   REVERB    OU..  > (B)


                   DELAY TIME     4096  SAMPLES

                     FEEDBACK     80 %

                      DRY MIX     100 %

                      WET MIX     60 %

          OVERFLOW PROTECTION     ON

cpu: 80 / 240 scanlines (30%)
```

## Sections

Disabled configs are greyed out with value `DISABLED`.

### STREAM

- Bit depth `[ 8, 16 ]`
- Half rate `[ ON, OFF ]`

### FILTER

- Type `[ LP 1-POLE (6db), LP 2-POLE (12db), LP 2-POLE RESONANT, LP 2-POLE RESONANT (FAST) ]`
- Cutoff freq `[20hz - 6000hz]`
- Q `[0.0 - 1.0]`

### ECHO

- Delay time `[ 0 - 8192 ]`
- Feedback `[ 0% - 100% ]`
- Dry mix `[ 0% - 100% ]`
- Wet mix `[ 0% - 100% ]`
- Overflow Protection `[ ON, OFF ]`

### REVERB

- Delay time `[ 0 - 8192 ]`
- Feedback `[ 0% - 100% ]`
- Dry mix `[ 0% - 100% ]`
- Wet mix `[ 0% - 100% ]`
- Overflow Protection `[ ON, OFF ]`

### DRIVE

- Gain `[ OFF, 2X ]`
- Overflow handling `[ SATURATE, OVERFLOW ]`

### OUTPUT

- Overflow Protection `[ON, OFF]`
