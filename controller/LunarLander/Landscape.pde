
import java.awt.geom.Line2D;

class Landscape {
  PMatrix2D matrix;   
  float [][] landscape = {
                              {-1000,1000},
                              {-1000,-1000},
                              {-1000,-100},
                              {    0,   0},
                              {   42, 188},
                              {   65, 153},
                              {  126, 153},
                              {  136, 167},
                              {  186, 224},
                              {  251, 144},
                              {  304, 144},
                              {  307,  88},
                              {  335,  70},
                              {  370, 234},
                              {  417, 234},
                              {  467, 146},
                              {  476, 164},
                              {  521, 164},
                              {  536, 144},
                              {  593, 185},
                              {  644, 129},
                              {  723, 129},
                              {  772, 239},
                              {  828, 239},
                              {  863, 106},
                              {  897, 106},
                              {  936, 210},
                              {  988, 210},
                              { 1026,  90},
                              { 1090,  90},
                              { 1133, 155},
                              { 1143, 184},
                              { 1120, 216},
                              { 1126, 234},
                              { 1165, 234},
                              { 1191, 184},
                              { 1227, 141},
                              { 1278, 127},
                              { 1316, 127},
                              { 1340, 161},
                              { 1435, 126},
                              { 1468, 126},
                              { 1494,   0},
                              { 2500,-100},
                              { 2500,-1000},
                              { 2505,1000}
                             };

  Landscape() {
  }

  void run() {
    render();
  }
  
  boolean checkCollision(Lander lander) {
    if (matrix == null) {
      return false;
    }
      
    PVector v1 = new PVector();
    PVector v2 = new PVector();
    PVector t1 = new PVector();
    PVector t2 = new PVector();
    for (int i = 0; i < landscape.length-1; i++) {
      for (int j = 0; j < lander.boundingBox.length-1; j++) {
        v1.x = landscape[i][0];      v1.y = landscape[i][1];
        v2.x = landscape[i+1][0];    v2.y = landscape[i+1][1];
        matrix.mult(v1, t1);
        matrix.mult(v2, t2);
        if (Line2D.linesIntersect(
          lander.boundingBox[j].x, lander.boundingBox[j].y,
          lander.boundingBox[j+1].x, lander.boundingBox[j+1].y,
          t1.x, t1.y,
          t2.x, t2.y)) {
          return true;
        }
      }
    }
    return false;
  }
  
  void render() {
    stroke(200);
    fill(20, 20, 20);
    pushMatrix();
    translate(0, 100);
    beginShape();
    for (int i = 0; i < landscape.length; i++) {
      vertex(landscape[i][0], landscape[i][1]);
    }
    endShape(CLOSE);
    fill(255, 0, 0);
    matrix = (PMatrix2D)getMatrix();
    popMatrix();
  }


}



