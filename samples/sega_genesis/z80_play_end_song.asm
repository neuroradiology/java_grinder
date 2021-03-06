.z80

FM1_REG_SEL equ 0x4000
FM1_REG_DATA equ 0x4001
FM2_REG_SEL equ 0x4002
FM2_REG_DATA equ 0x4003

KEY_PRESS equ 0x28
CH1_FREQ_LSB equ 0xa0
CH1_FREQ_MSB equ 0xa4
CH1_SOUND_ON equ 0xb4

PSG_DATA equ 0x7f11

start:
  ld sp, 0x2000

  ld ix, setup
  ld c, 0x30
  ld b, setup_end - setup

load_setup:
  call init_channel
  call init_channel
  call init_channel
  inc c
  inc ix
  djnz load_setup

  call wait_busy
  ld a, CH1_SOUND_ON
  ld (FM1_REG_SEL), a
  ld a, 0xc0
  ld (FM1_REG_DATA), a

  call wait_busy
  ld a, CH1_SOUND_ON+1
  ld (FM1_REG_SEL), a
  ld a, 0xc0
  ld (FM1_REG_DATA), a

  call wait_busy
  ld a, CH1_SOUND_ON+2
  ld (FM1_REG_SEL), a
  ld a, 0xc0
  ld (FM1_REG_DATA), a

  call wait_busy
  ld a, CH1_SOUND_ON
  ld (FM2_REG_SEL), a
  ld a, 0xc0
  ld (FM2_REG_DATA), a

  call wait_busy
  ld a, CH1_SOUND_ON+1
  ld (FM2_REG_SEL), a
  ld a, 0xc0
  ld (FM2_REG_DATA), a

  call wait_busy
  ld a, CH1_SOUND_ON+2
  ld (FM2_REG_SEL), a
  ld a, 0xc0
  ld (FM2_REG_DATA), a

.if 0
  ld a, CH1_FREQ_MSB
  ld (FM1_REG_SEL), a
  ld a, 0x22
  ld (FM1_REG_DATA), a

  ld a, CH1_FREQ_LSB
  ld (FM1_REG_SEL), a
  ld a, 0x69
  ld (FM1_REG_DATA), a

  ld a, KEY_PRESS
  ld (FM1_REG_SEL), a
  ld a, 0xf0
  ld (FM1_REG_DATA), a
.endif

  ;; Turn volume off for all voices of PSG
  ld a, 0x8f | (0 << 5) | (1 << 4)
  ld (PSG_DATA), a
  ld a, 0x8f | (1 << 5) | (1 << 4)
  ld (PSG_DATA), a
  ld a, 0x8f | (2 << 5) | (1 << 4)
  ld (PSG_DATA), a
  ld a, 0x8f | (3 << 5) | (1 << 4)
  ld (PSG_DATA), a

  ;; Done setting up, now play song
  ld d, 0

  ld iy, song
next_pattern:
  ;; hl = next pattern from list
  ld l, (iy)
  ld h, (iy+1)
  ld a, h
  ;; if pattern == 0, break
  or l
  jp z, song_end

play_pattern:
  ;; read in 3 bytes (time=a, channel=a, note=c) from &hl
  ld a, (hl)
  inc hl

  ;; delay 'a' times
play_pattern_delay:
  cp 0
  jp z, play_pattern_delay_end
  ld d, 50                 ; this can control tempo
delay_loop_outer:
  ld b, 200
delay_loop:
  djnz delay_loop          ; should be 13 cycles per loop
  dec d
  jp nz, delay_loop_outer
  dec a
  jp play_pattern_delay
play_pattern_delay_end:

  ld a, (hl)
  inc hl
  ;; if a(channel) == 0xff, break
  cp 0xff
  jp z, end_pattern
  ld b, a

  ;; Convert MIDI frequency to Yahama constant
  ld e, (hl)
  inc hl
  ld ix, frequency
  add ix, de 
  add ix, de

  ;; Call correct code for channels 0-2 or 3-5
  cp 0x03
  jp nc, channels345

channels012:
  ld c, a

  call wait_busy
  ld a, KEY_PRESS
  ld (FM1_REG_SEL), a
  ld a, 0x00
  add a, b
  ld (FM1_REG_DATA), a

  call wait_busy
  ld a, CH1_FREQ_MSB
  add a, c
  ld (FM1_REG_SEL), a
  ld a, (ix+1)
  ld (FM1_REG_DATA), a

  call wait_busy
  ld a, CH1_FREQ_LSB
  add a, c
  ld (FM1_REG_SEL), a
  ld a, (ix)
  ld (FM1_REG_DATA), a

  call wait_busy
  ld a, KEY_PRESS
  ld (FM1_REG_SEL), a
  ld a, 0xf0
  add a, b
  ld (FM1_REG_DATA), a

  jp play_pattern

channels345:
  cp 0x07
  jp z, channels7

  sub 3
  ld c, a

  call wait_busy
  ld a, KEY_PRESS
  ld (FM1_REG_SEL), a
  ld a, 0x00
  add a, b
  ld (FM1_REG_DATA), a

  call wait_busy
  ld a, CH1_FREQ_MSB
  add a, c
  ld (FM2_REG_SEL), a
  ld a, (ix+1)
  ld (FM2_REG_DATA), a

  call wait_busy
  ld a, CH1_FREQ_LSB
  add a, c
  ld (FM2_REG_SEL), a
  ld a, (ix)
  ld (FM2_REG_DATA), a

  call wait_busy
  ld a, KEY_PRESS
  ld (FM1_REG_SEL), a
  ld a, 0xf0
  add a, b
  ld (FM1_REG_DATA), a

  jp play_pattern

channels7:
  ld a, e

  cp 0x00
  jp nz, psg_not_off

  ;; Turn PSG noise voice off
  ld a, 0xff
  ld (PSG_DATA), a
  jp play_pattern

psg_not_off:
  ;; Turn PSG noise voice on
  ld a, 0xf0
  ld (PSG_DATA), a

  ;; 38 = bass, 36=snare
  cp 36
  jp z, psg_snare_drum

psg_bass_drum:
  ld a, 0xe4
  ld (PSG_DATA), a
  jp play_pattern

psg_snare_drum:
  ld a, 0xe7
  ld (PSG_DATA), a
  jp play_pattern

end_pattern:
  inc iy
  inc iy
  jp next_pattern

song_end:

while_1:
  jp while_1

init_channel:
  call wait_busy
  ld a, c
  ld (FM1_REG_SEL), a
  ld a, (ix)
  ld (FM1_REG_DATA), a
  call wait_busy
  ld a, c
  ld (FM2_REG_SEL), a
  ld a, (ix)
  ld (FM2_REG_DATA), a
  inc c
  ret

wait_busy:
  ld a, (FM1_REG_SEL)
  bit 7, a
  jp nz, wait_busy
  ret

setup:
  db 0x71, 0x0d, 0x33, 0x01
  db 0x23, 0x2d, 0x26, 0x00
  db 0x5f, 0x99, 0x5f, 0x94
  db 0x05, 0x05, 0x05, 0x07
  db 0x02, 0x02, 0x02, 0x02
  db 0x11, 0x11, 0x11, 0xa6
  db 0x00, 0x00, 0x00, 0x00
setup_end:

.include "freq_chart.inc"

intro:
  db 0x00, 0x00, 0x41, 0x00, 0x01, 0x39, 0x00, 0x02, 0x3c, 0x30, 0x00, 0x41,
  db 0x00, 0x01, 0x39, 0x00, 0x02, 0x3e, 0x30, 0x00, 0x41, 0x00, 0x01, 0x39,
  db 0x00, 0x02, 0x40, 0x30, 0x00, 0x41, 0x00, 0x01, 0x38, 0x00, 0x02, 0x41,
  db 0x30, 0xff, 0xff,
pattern_1:
  db 0x00, 0x00, 0x39, 0x00, 0x01, 0x3c, 0x00, 0x02, 0x40, 0x60, 0x00, 0x3c,
  db 0x00, 0x01, 0x40, 0x00, 0x02, 0x43, 0x60, 0xff, 0xff,
pattern_2:
  db 0x00, 0x00, 0x45, 0x00, 0x01, 0x48, 0x00, 0x02, 0x4c, 0xc0, 0xff, 0xff,
song:

  dw intro, intro, intro, intro,
  dw pattern_1, pattern_1, pattern_1, pattern_1,
  dw pattern_2, pattern_2, pattern_2, pattern_2,
  dw 0

