/* Handle messages from TG api */
void handleNewMessages(int numNewMessage) {
  Serial.print("Handle New Message: ");
  Serial.println(numNewMessage);

  for(int i=0; i<numNewMessage; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if(chat_id == su_ID) {          // obtain su level
      bot.sendMessage(chat_id, "$sudo$", "");
      su_login = true;
    }
    else {
      su_login = false;
    }

    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;
    if (text == "/start") {
      String welcome = "Welcome to use iLocker, " + from_name + "\n";
      welcome += "Use the following commands to contorl your locker \n";
      // welcome += "/photo : takes a new photo\n";
      // welcome += "/flash : toggles flash LED \n";
      welcome += "/unlock : unlocks your locker \n";
      welcome += "/sitrep : checks states of each device \n";
      welcome += "/reserve : reserves a locker for you\n";
      bot.sendMessage(chat_id, welcome, "");
      Serial.printf("New user joined: %s\n", &chat_id[0]);
    }

    if (text == "/sitrep") {
      String resp = "Main controller, Check! \n";
      Serial.println(millis());

      if(millis() - last_camIn_check < 10000) 
        resp += "Inside Camera, Check! \n";
                                                  
      if(millis() - last_camOut_check < 10000) 
        resp += "Outside Camera, Check! \n";
      
      Serial.println(resp);
      bot.sendMessage(chat_id, resp, "");
    }

    if (text == "/unlock") {
      String resp = "";
      if(lock_state) {
        resp += "Locker is unlocked, enjoy your food~";
        lock_state = 0;
        digitalWrite(LOCK_pin, lock_state);
        GL_ONtime = millis();
        current_user.clear();
        Serial.println(resp);
        bot.sendMessage(chat_id, resp, "");
      }
      else {
        resp += "Nothing's in locker.";
        Serial.println(resp);
        bot.sendMessage(chat_id, resp, "");
      }
    }

    if (text == "/reserve") {
      String resp = "";
      if(lock_state && current_user != chat_id) {
        resp += "Sorry! It has been reserved.";
        Serial.println(resp);
        bot.sendMessage(chat_id, resp, "");
      }
      else {
        resp += "Your locker is no.23";
        current_user += chat_id;
        Serial.println(resp);
        //Serial.println(current_user);
        bot.sendMessage(chat_id, resp, "");
        UDPsent(1);          // assign to the user
      }
    }
  }
}
