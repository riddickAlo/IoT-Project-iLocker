 /* Control LED state */
void LED_state(unsigned long tstamp) {
  if(lock_state){           // locked, RL on
    digitalWrite(RL_pin, 1);
    digitalWrite(YL_pin, 0);
    digitalWrite(GL_pin, 0);
  }
  else {                    // being reserved, YL on
    digitalWrite(RL_pin, 0);
    if(strlen(&current_user[0]) > 0) {
      digitalWrite(YL_pin, 1);
      digitalWrite(GL_pin, 0);
    }
    else {                  // unlocked & available, GL on
      digitalWrite(YL_pin, 0);
      digitalWrite(GL_pin, 1);
    }
  }
}
