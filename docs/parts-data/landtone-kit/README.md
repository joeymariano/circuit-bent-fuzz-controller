# Prototype platform — LANDTONE DIY Fuzz kit

This circuit-bent controller is being prototyped against a **LANDTONE "Build Your Own
Fuzz Pedal" DIY kit** in a 1590B-style aluminum enclosure.

Product: [Build Your Own Fuzz Pedal — 1590B Aluminum Stomp Box (B06XQL49G8)](https://www.amazon.com/dp/B06XQL49G8)
(LANDTONE sells several near-identical fuzz/distortion kits across Amazon, Reverb and eBay.)

## Kit specs

- DIY **fuzz / distortion** pedal, op-amp based, **through-hole** parts
- **1590B**-style aluminum enclosure
- **3PDT footswitch, true bypass**
- Runs on a **9 V battery** or a standard **negative-tip 9 V** supply
- Numbered PCB (each part has a matching number on the board)
- Controls: **Volume (VOL)** and **Drive (DRV)** — see the enclosure SVGs in the repo root

## How this project ties in

The XIAO controller board taps three points on the assembled pedal:

- **T1 / INTRPT** → interrupts the pedal's **main LED** (in series with the 3PDT's
  LED-ground return), used here as the mode indicator.
- **T2 / BND-1** and **T3 / BND-2** → two circuit-**bend** points on the fuzz PCB.

See the repo-root `README.md` for the pin map and wiring notes, and
`../../Landtone-Fuzz-Wiring` references for how the stock pedal is wired.

## Photos

Drop reference photos of your build here (e.g. `board.jpg`, `enclosure.jpg`,
`wiring.jpg`). See the note in the main README about the product-listing images.
