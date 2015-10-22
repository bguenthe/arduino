
import ddf.minim.*;
import java.awt.geom.Line2D;

class Lander {

  static final int MODE_FLOATING = 0;
  static final int MODE_THRUSTING = 1;
  static final int MODE_LANDED = 2;
  static final int MODE_CRASHED = 3;
  
  static final int INPUT_ROCKET_OFF = 1;
  static final int INPUT_ROCKET_ON = 2;
  static final int INPUT_ROTATE_L = 3;
  static final int INPUT_ROTATE_R = 4;
  static final int INPUT_ROTATE_A = 5;
  static final int INPUT_CRASH = 6;
  static final int INPUT_LAND = 7;
  
  int mode = MODE_FLOATING;

  PVector[] boundingBox;  
  float[][] boundingBoxPoints = { 
      { -3.0, -10.0},
      {  3.0, -10.0},
      { 10.0,  10.0},
      {-10.0,  10.0}
    };

  Landscape landscape;
  PVector location;
  PVector velocity;
  PVector acceleration;
  PVector rocketAcceleration;
  float direction;
  float fuel = 100.0;
  AudioSnippet mainRocket;
  AudioSample steeringRocket;
  AudioSample fanfare;
  AudioSample explosion;
  PMatrix2D matrix;
  
  Lander(float x, float y, Landscape landscape) {
    this.landscape = landscape;
    location = new PVector(x, y, 0);
    velocity = new PVector(0.1, 0, 0);
    acceleration = new PVector(0, 0.005, 0);
    rocketAcceleration = new PVector(0, -0.015, 0);
    direction = 0;
    mainRocket = minim.loadSnippet("rocket_main.wav");
    steeringRocket = minim.loadSample("rocket_steering.wav");
    fanfare = minim.loadSample("fanfare.wav");
    explosion = minim.loadSample("explosion.wav");
    boundingBox = new PVector[5];
    for (int i = 0; i < 5; i++) {
      boundingBox[i] = new PVector();
    }
  }

  void run() {
    update();
    collide();
    render();
  }

  void collide() {
    if (((mode == MODE_FLOATING) || (mode == MODE_THRUSTING)) && landscape.checkCollision(this)) {
      println("veloctiy v:" + velocity.mag() + " x:" + velocity.x + " mode:" + mode + " fuel:" + fuel);
      if ((velocity.mag() < 0.3) && (abs(velocity.x) < 0.15) && (abs(direction) < 0.1)) {
        input(INPUT_LAND);
      }
      else {
        input(INPUT_CRASH);
      }
      velocity.x = 0;
      velocity.y = 0;
      acceleration.x = 0;
      acceleration.y = 0;
    }
  }
  
  void update() {
    if ((mode == MODE_LANDED) || (mode == MODE_CRASHED)) {
      return;
    }
    if (mode == MODE_THRUSTING) {
      if (fuel <= 0.0) {
        input(INPUT_ROCKET_OFF);
      }
      else {
        fuel -= 0.05;
        PVector v = rocketAcceleration.get();
        v.x = cos(direction)*v.x - sin(direction)*v.y;
        v.y = sin(direction)*v.x + cos(direction)*v.y;
        velocity.add(v);
      }
    }    
    velocity.add(acceleration);
    location.add(velocity);
  }
  
  
  void input(int input) {
    input(input, 0.0);
  }
  
  void input(int input, float value) {
    switch (mode) {
      case MODE_FLOATING:
        switch (input) {
          case INPUT_ROCKET_ON:
            if (fuel > 0.0) {
              mode = MODE_THRUSTING;
              if (!mainRocket.isPlaying()) {
                mainRocket.loop();
              }
            }
            else {
              mode = MODE_FLOATING;
              mainRocket.pause();
            }
            break;
          case INPUT_ROTATE_L:
            direction -= 0.1;
            steeringRocket.trigger();
            break;
          case INPUT_ROTATE_R:
            direction += 0.1;
            steeringRocket.trigger();
            break;
          case INPUT_ROTATE_A:
            direction = value;
            break;
          case INPUT_CRASH:
            println("boooom!!!");
            explosion.trigger();
            mode = MODE_CRASHED;
            break;
          case INPUT_LAND:
            println("tadaa!");
            fanfare.trigger();
            mode = MODE_LANDED;
          }
        break;
      case MODE_THRUSTING:
        switch (input) {
          case INPUT_ROCKET_OFF:
            mode = MODE_FLOATING;
            mainRocket.pause();
            break;
          case INPUT_ROTATE_L:
            direction -= 0.1;
            steeringRocket.trigger();
            break;
          case INPUT_ROTATE_R:
            direction += 0.1;
            steeringRocket.trigger();
            break;          
          case INPUT_ROTATE_A:
            direction = value;
            break;
          case INPUT_CRASH:
            println("boooom!!!");
            mainRocket.pause();
            explosion.trigger();
            mode = MODE_CRASHED;
            break;
          case INPUT_LAND:
            println("tadaa!");
            mainRocket.pause();
            fanfare.trigger();
            mode = MODE_LANDED;         
        }
        break;
      case MODE_LANDED:
        break;
      case MODE_CRASHED:
        break;
      default:
        ;
    }
  }
  
  void rocketOff() {
    input(INPUT_ROCKET_OFF);
  }

  void rocketOn() {
    input(INPUT_ROCKET_ON);
  }
    
  void rotateLeft() {
    input(INPUT_ROTATE_L);
  }
  
  void rotateRight() {
    input(INPUT_ROTATE_R);
  }
  
  void rotateAngle(float angle) {
    input(INPUT_ROTATE_A, angle);
  }

  void stop() {
    mainRocket.close();
    steeringRocket.close();
    fanfare.close();
    explosion.close();
  }
  
  void render() {
    
    // fuel gauge
    fill(0);
    stroke(255);
    rect(5, 5, 10, 40);
    fill(50, 100, 255);
    float h = (40.0/100.0) * fuel; 
    rect(5, 5 + 40-h, 10, h);
    
    pushMatrix();
    translate(location.x, location.y);
    rotate(direction);
    
    if (mode == MODE_THRUSTING) {
      fill(255, 150, 150);
      stroke(255, 50, 50);
      beginShape();
      vertex(-2, 7);
      vertex( 0, 14);
      vertex(2, 7);
      endShape(CLOSE);
    }
    
    stroke(255);
    fill(100, 100, 100);
    beginShape();
    vertex(-3, -1);
    vertex(-6, -4);
    vertex(-6, -7);
    vertex(-3, -10);
    vertex(3, -10);
    vertex(6, -7);
    vertex(6, -4);
    vertex(3, -1);
    endShape(CLOSE);
    
    beginShape();
    vertex(-6, -1);  // body
    vertex(6, -1);
    vertex(6, 4);
    vertex(-6, 4);
    endShape(CLOSE);
    noFill();
    
    beginShape();
    vertex(6, 1);  // upper leg
    vertex(8, 7);
    vertex(4, 4);
    endShape();

    beginShape(LINES);
    vertex(8, 7);   // leg
    vertex(8, 10);
    vertex(6, 10);  // landing pad
    vertex(10, 10);
    endShape();

    beginShape();
    vertex(-6, 1);  // upper leg
    vertex(-8, 7);
    vertex(-4, 4);
    endShape();

    beginShape(LINES);
    vertex(-8, 7);   // leg
    vertex(-8, 10);
    vertex(-6, 10);  // landing pad
    vertex(-10, 10);
    endShape();
    
    beginShape();
    vertex(0, 4);
    vertex(-2, 7);
    vertex(2, 7);
    endShape(CLOSE);

    matrix = (PMatrix2D)getMatrix();   
    for (int i = 0; i < 4; i++) { 
      PVector source = new PVector(boundingBoxPoints[i][0], boundingBoxPoints[i][1]);
      matrix.mult(source, boundingBox[i]);
    }
    boundingBox[4] = boundingBox[0];
    popMatrix();
        
  }

}


