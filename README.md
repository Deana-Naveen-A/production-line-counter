# Production Line Counter

An Arduino-based counter for production lines using an IR beam-break sensor.

## Components
- Arduino Uno
- IR beam-break sensor (LM393)
- 16×2 LCD with I2C backpack


## How it works
Items passing on a conveyor break an IR beam. Each break increments a counter
displayed on the LCD, along with a live items/min rate. A reset button clears the count.

## Files
- `production_counter.ino` — Arduino source code
- `production_line_report.docx` — Full project report with circuit diagram
<img width="3024" height="4032" alt="IMG_34E22B75-7CB0-4921-8C0D-81B9436F3B09" src="https://github.com/user-attachments/assets/de30e7f6-fefc-496d-80b4-bb3af1199c19" />
<img width="5712" height="4284" alt="IMG_7610" src="https://github.com/user-attachments/assets/caef649d-8ec4-4c5e-a096-a18781e84a7a" />
<img width="4284" height="5712" alt="IMG_7609" src="https://github.com/user-attachments/assets/7146b5df-62e1-46a3-be39-b1ea8b11def8" />
<img width="3024" height="4032" alt="IMG_34E22B75-7CB0-4921-8C0D-81B9436F3B09 (1)" src="https://github.com/user-attachments/assets/14268f1e-53e7-47e6-9d89-5c35b20443bb" />


## Circuit
| Component | Connects to |
|---|---|
| IR sensor OUT | Arduino D2 |
| Reset button | Arduino D3 |
| LCD SDA | Arduino A4 |
| LCD SCL | Arduino A5 |

