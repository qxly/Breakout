collision:
1.If the ball goes too fast, it might skip over an object entirely within a single frame, not detecting any collisions.
2.If the ball hits more than one object within a single frame, it will have detected two collisions and reverse its velocity twice; not affecting its original velocity.
3.Hitting a corner of a brick could reverse the ball's velocity in the wrong direction since the distance it travels in a single frame could make the difference between VectorDirection returning a vertical or horizontal direction.