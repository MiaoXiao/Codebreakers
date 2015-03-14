# Codebreakers
Codebreakers is a two player video game built on an embedded system with two ATmega128 microcontrollers, two keypads, and two LCD screens.
Two players compete by being the first to guess a randomly generated string of characters at the start of every game. However, both players will have opportunities to confuse their opponents during gameplay by using powerups.
To excel at this game, one must master concentration, memory, and dexterity!

## How to win
Both players must enter a randomly generated code on their keypad in order to win. When a player enters a correct character in the right sequence, their side of the LED display will light up in order.
When all 5 LEDS light up on the corresponding player's side, that player will win the game!
If an incorrect character in the sequence is inputted, the player's side of the LED display will reset. This way, both players will eventually be able to correctly input the entire code, given that they can remember the entire combination.

## Power Ups
`A`: Attack. <br> Freezes your opponents screen for 10 seconds.<br>
`B`: Bug. <br> Flips all the digits on your opponents keypad for 30 seconds. Which means when your opponent presses a digit on their keypad, it will lead to a different digit. `1` will correspond to `2`, `3` will correspond to `4`, `5` will correspond to `6`, `7` will correspond to `8`, and `9` will correspond to `0`, all vice versa. <br>
`C`: Counter. <br> The player will be immune to any of your opponents harmful powerups for 20 seconds. Using this powerup will also remove Bug from your keypad, if necessary. <br>
`D`: Disrupt. <br> Will randomly change one digit in your opponents code, which means your opponent will have to find the new correct digit for their sequence. <br>

At the bottom left of both screens, is the Powerup String, which is a series of 4 random characters. If at any point this string changes during the course of a game, any player may enter:

`**`

to obtain a random powerup! Note that if a player tries to grab a powerup but the Powerup String has NOT changed, the player's own screen will be frozen for 10 seconds. Not good!

When you obtain a powerup, a letter either `A`, `B`, `C`, or `D` will appear on the top right of your LCD. To use a powerup, simply enter `#` then the letter of the powerup you want to use! For example, to use the power up Bug:

`#B`

## Strategy
Although powerups are very strong and can seriously confuse your opponent, it takes a lot of concentration to be able to get one without freezing your own screen, which will make you slower at entering the correct code.
Likewise, completely ignoring the Powerup String and only focusing on entering the code may lead you unable to defend yourself from attack!
Saving your power ups if you have some is also a concern, as using all of them at once might waste them if your opponent has Counter up.
