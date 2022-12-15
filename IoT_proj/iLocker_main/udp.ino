void handleUDPmessage() {
  if(UDP1.parsePacket()) {
    UDP1.read(packet, 10); 
    last_camIn_check = millis();
    Serial.printf("Cam_IN: %s\n", packet);

  }
  if(UDP2.parsePacket()) {
    UDP2.read(packet, 10); 
    last_camOut_check = millis();
    Serial.printf("Cam_Out: %s\n", packet);
  }
}

void UDPsent(uint8_t mode) {
  /*
    mode 0: send "/c", ping nodes.
    mode 1: send "/u, {user_id}", tell camera nodes who is the reserved user.
    mode 2: send "/s", tell camera nodes to take a picture.
  */
  String sentPacket = "";
  switch(mode) {
    case 0:
      sentPacket += "c";
      break;
    case 1:
      sentPacket += "u,";
      sentPacket += current_user;
      break;
    case 2:
      sentPacket += "s";
      break;
    default:
      Serial.println("Error mode!!");
      sentPacket += "c";
      break;
  }
  sentPacket += '/';                  // in our protocol, '/' means the end of message
  
  if(sentPacket != old_pak) {
    Serial.println(sentPacket);
    old_pak = sentPacket;
  }
  UDP1.beginPacket(CamIn_IP, UDP_PORT1);
  UDP1.write(&sentPacket[0]);
  UDP1.endPacket();

  delay(50);
  handleUDPmessage();

  UDP2.beginPacket(CamOut_IP, UDP_PORT1);
  UDP2.write(&sentPacket[0]);
  UDP2.endPacket();

  delay(50);
  handleUDPmessage();
}