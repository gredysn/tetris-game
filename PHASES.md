# 🧩 Project Phases — PA3: Tetris
---

## Phase 1: Warm-Up *(Suggested Deadline: Nov 13)*
**“Let’s get started!”**  
Get familiar with the base code and implement the fundamental mechanics of the game.

### 🧠 Tasks

- **Switch It Up**  
  Add extra movement controls (recommended keys): WASD
➤ Make sure to update the controls display on the right panel.

- **Add Some Color**  
Assign a unique color to each piece — no duplicates allowed!

- **Add Some Background**  
Implement a background image. You can use the default one in `bin/data` or choose your own.  
(*Make sure it’s appropriate!*)

- **Add Music**  
Include background music using the provided sound file (or your own).  

- **Mute / Unmute**  
Add functionality to mute and unmute the background music.  
Music should stay muted even after starting a new game.  
➤ Update the controls display accordingly.

- **Rotate**  
Fix the missing rotation functionality in `piece.cpp`.  
*(Hint: check how pieces are defined — there’s a variable that helps! 👀)*

---

## Phase 2: Functionality *(Suggested Deadline: Nov 20)*
**“Now it’s time to add the main functions so we can Tetris!”**

### ⚙️ Tasks

- **Implement `placePiece()`**  
Currently, pieces disappear when they hit the bottom. Fix this so they stay in place!

- **Implement `GameOver`**  
Add logic to trigger a Game Over state when new pieces can no longer spawn at the top.  
*(There’s already an empty function in `board.cpp` for this.)*

- **Add `hardDrop()`**  
When pressing `Q`, the piece should instantly fall to the bottom.  
➤ Remember to update the controls display.

- **Faster, Faster, FASTER!**  
As the score increases, make the blocks fall faster.  
You can do this gradually or by levels — just don’t make it unplayable.  
*(Tip: use the `L` key to test score speed scaling.)*

---

## Phase 3: The Fun Stuff 😄 *(Suggested Deadline: Nov 27)*
**“Time to polish and make it shine.”**

### 🎮 Tasks

- **Implement `switchToPrevPiece()`**  
Behavior when pressing `P`:
- If there’s **no previous piece**, swap the **current** and **next** piece once.  
- If there **is** a previous piece, switch with that one instead.  
- You don’t need to modify `drawPreviousPiece()` — just ensure logic correctness.  
➤ Add this control to the display on the right.

- **Ghost Piece 👻**  
Complete `drawGhostPiece()` to draw where the current piece would land if hard dropped.

- **New Piece 🧱**  
Create a brand-new piece (not an existing one and not just one cell).  
Define **all its rotations** in `piece.cpp`.


> Bonus points are given at the **TA’s discretion** — be creative and make it your own!


