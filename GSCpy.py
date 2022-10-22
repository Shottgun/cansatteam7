newFileC = open("flightData.csv", "a")
newFileT = open("flightData.txt", "a")
pastTransmission = ''
payloadStage = ''
message = ''

def readSD():
    global datalog
    # Opening the file with absolute path
    flightData = open(r'D:\LOG00018.txt', 'r')
    # read file
    datalog = flightData.read()
    # Closing the file after reading
    flightData.close()

def readCSV(message):
    global teamID, missionTime, packetNum, flightStage, payloadState, alt, temp, pres, voltage, lat, long, roll, pitch, yaw
    teamID = ''
    missionTime = ''
    packetNum = ''
    flightStage = ''
    payloadState = ''
    alt = ''
    temp = ''
    pres = ''
    voltage = ''
    lat = ''
    long = ''
    roll = ''
    pitch = ''
    yaw = ''
    varsSet = 1
    for i in range(len(str(message))):
        if str(message)[i] == '|':
            pass
        elif str(message)[i] != ',':
            match varsSet:
                case 1: teamID = teamID + str(message)[i]
                case 2: missionTime = missionTime + str(message)[i]
                case 3: packetNum = packetNum + str(message)[i]
                case 4: flightStage = flightStage + str(message)[i]
                case 5: payloadState = payloadState + str(message)[i]
                case 6: alt = alt + str(message)[i]
                case 7: temp = temp + str(message)[i]
                case 8: pres = pres + str(message)[i]
                case 9: voltage = voltage + str(message)[i]
                case 10: lat = lat + str(message)[i]
                case 11: long = long + str(message)[i]
                case 12: roll = roll + str(message)[i]
                case 13: pitch = pitch + str(message)[i]
                case 14: yaw = yaw + str(message)[i]          
        else:
            varsSet+=1

def fileWriteDataTXT(teamID, missionTime, packetNum, flightStage, payloadState, alt, temp, pres, voltage, lat, long, roll, pitch, yaw):
    for i in range(15):
        match i:
            case 1: 
                print("Team ID = ", end = ''), print(teamID),
                newFileT.write("Team ID = "),  newFileT.write(teamID), newFileT.write('\n')
            case 2: 
                print("Mission Time = ", end = ''), print(missionTime, end = ''), print(" hh:mm:ss") 
                newFileT.write("Mission Time = "),  newFileT.write(missionTime), newFileT.write(" hh:mm:ss"), newFileT.write('\n')
            case 3: 
                print("Packet's Sent = ", end = ''), print(packetNum)
                newFileT.write("Packet's Sent = "),  newFileT.write(packetNum), newFileT.write('\n')
            case 4: 
                print("Current Flight Stage = ", end = ''), print(flightStage)
                newFileT.write("Current Flight Stage = "),  newFileT.write(flightStage), newFileT.write('\n')
            case 5: 
                print("Current Payload State = ", end = ''), print(payloadState)
                newFileT.write("Current Payload State = "),  newFileT.write(payloadState), newFileT.write('\n')
            case 6: 
                print("Altitude = ", end = ''), print(alt, end = ''), print(" m")
                newFileT.write("Altitude = "),  newFileT.write(alt), newFileT.write(" m"), newFileT.write('\n')
            case 7: 
                print("Temperature = ", end = ''), print(temp, end = ''), print(" *C")
                newFileT.write("Temperature = "),  newFileT.write(temp), newFileT.write(" *C"), newFileT.write('\n')
            case 8: 
                print("Pressure = ", end = ''), print(pres, end = ''), print(" hPa")
                newFileT.write("Pressure = "),  newFileT.write(pres), newFileT.write(" hPa"), newFileT.write('\n')
            case 9: 
                print("Voltage = ", end = ''), print(voltage, end = ''), print(" V")
                newFileT.write("Volatge = "),  newFileT.write(voltage), newFileT.write(" V"), newFileT.write('\n')
            case 10: 
                print("Latitude = ", end = ''), print(lat)
                newFileT.write("Latitude = "),  newFileT.write(lat), newFileT.write('\n')
            case 11: 
                print("Longitude = ", end = ''), print(long)
                newFileT.write("Longitude = "),  newFileT.write(long), newFileT.write('\n')
            case 12: 
                print("Roll(x) = ", end = ''), print(roll, end = ''), print(" degrees")
                newFileT.write("Roll(x) = "),  newFileT.write(roll), newFileT.write(" degrees"), newFileT.write('\n')
            case 13: 
                print("Pitch(y) = ", end = ''), print(pitch, end = ''), print(" degrees")
                newFileT.write("Pitch(y) = "),  newFileT.write(pitch), newFileT.write(" degress"), newFileT.write('\n')
            case 14: 
                print("Yaw(z) = ", end = ''), print(yaw, end = ''), print(" degrees\n")
                newFileT.write("Yaw(z) = "),  newFileT.write(yaw), newFileT.write(" degrees"), newFileT.write('\n\n')

def fileWriteCSV(message):
    global toWrite
    toWrite = ''
    for i in range(len(str(message))):
        if str(message)[i] == '|':
            pass
        else:
            toWrite = toWrite + message[i]
    newFileC.write(str(toWrite))

readSD()

###main :D
for i in range(len(str(datalog))):
    message = message + str(datalog)[i]
    if str(datalog)[i] == '|':
        readCSV(message)
        fileWriteCSV(message)
        fileWriteDataTXT(teamID, missionTime, packetNum, flightStage, payloadState, alt, temp, pres, voltage, lat, long, roll, pitch, yaw)
        message = ''

print("Flight Complete :D")
newFileC.close()
newFileT.close()
