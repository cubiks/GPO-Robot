/*
  RobotHelloWorld

  V0.9b  JpD 29-08-2016
         Removed neopixel code
  
  V0.9a  JpD 29-08-2016
         Corrected negative turn
  
  V0.8   JpD & TmD 29-08-2016
         Callback via pointer
  
  V0.7   JpD 28-08-2016
         More comments added
  
  V0.6   JpD 27-08-2016
         More Cleaning, comments added
  
  V0.5   JpD 27-08-2016
         Cleaned, corrections for errors when stopping
  
  V0.4   JpD & JnD 21-08-2016
         Distance sensor corrections.

  V0.3   JpD & JnD 21-08-2016
         Distance measured during run. Sequence from loop.

  V0.2   JpD & JnD 21-08-2016
         Turn implemented, Slow and Fast implemented.

  V0.1   Jpd 21-08-2016
         Name changed to RobotHelloWorld. Stop using short reverse burst
*/

//-----------------------------------------------------------------------------------------
// Motor Direction Values

 #define STOP     0 // 
 #define FORWARD  1 //
 #define REVERSE  2 //

// Speed and Motor PWM Slow
 #define SLOW 250 // Speed in mm/sec for slow drive
 #define SLOWPWM 80 // PWM value for slow drive (experiment with setting for your robot)

// Speed and Motor PWM Fast
 #define FAST 650 // Speed in mm/sec for fast drive
 #define FASTPWM 255 // PWM value for fast drive ( max pwm )

//
#define SCAN_TURN_DEGREES 15
#define SCAN_TURN_MAX_TIMES 24

// millimeters
#define SCOUT_DISTANCE 500
#define KNOCK_DISTANCE 100

//-----------------------------------------------------------------------------------------
// Typedefs
 typedef void (*ISRCallBack)( byte, int, int ); // callback declaration


//-----------------------------------------------------------------------------------------
void setup() 
{
  Serial.begin(115200); // set up Serial library at 115200 bps
  Serial.println("Robot Hello World!");
  
  speedSensorInit( driveSpeedSensorCallback );
  distanceSensorInit();
  motorControlInit();
  
}


//-----------------------------------------------------------------------------------------
void loop() 
{

  static int action = 1;
  static int lastAction = 0;
  static int turnCount = 0;
  static int setDistance = 0;

  switch ( action )
  {
    case 1:
    { // init for drive
      Serial.print("action: ");
      Serial.println( action );
      lastAction = action;
      speedSensorClear(); // clear counts
      setDistance = SCOUT_DISTANCE; // set distance for drive in mm.
      action = 2; // drive forward
      break;
    }

    case 2:
    { // drive slow
      Serial.print("action: ");
      Serial.println( action );
      lastAction = action; // remeber where we are
      if ( !driveStraight( FORWARD, setDistance, SLOW, 5 ) ) // drive: slow , detect obstacles
      { // obstacle detected
        setDistance = setDistance - driveDistanceDone(); // calculate the rest of the distance to go.
        action = 10; // react to obstacle in action 10
        break;
      }
      action = 3;
      break;
    }

    case 3:
    { // scan
      Serial.print("action: ");
      Serial.println( action );
      lastAction = action;

      driveStop(); // stop
      delay(500);
      driveTurn(SCAN_TURN_DEGREES); // turn a little bit
      driveStop(); // stop

      if (distanceSensorCheckObstacle(SCOUT_DISTANCE)) {
        action = 2;
        turnCount = 0;
        setDistance = SCOUT_DISTANCE;
      } else {
        turnCount++; // remember how many times we did this.
        action = 3; // keep scanning
      }
      break;
    }

    case 10:
    { // an obstacle is detected: stop and wait for it to go away
      Serial.print("action: ");
      Serial.println( action );
      driveStop(); // stop
      delay(500);
      driveTurn(180); // turn around
      driveStop(); // stop
      delay(500);
      driveStraight( REVERSE, KNOCK_DISTANCE, SLOW, 1 );
      driveStop(); // stop
      delay(500);
      action = 3; // obstacle down, start scanning for others.
      break;
    }

    default:
      break; 
  }

  // We turned 360 degrees but no obstacles found
  if (turnCount >= SCAN_TURN_MAX_TIMES) {
    turnCount = 0; // reset counter
    action = 2; // Move forward then resume scanning
    setDistance = SCOUT_DISTANCE; // 
  }

  delay(100);
}

// end module
