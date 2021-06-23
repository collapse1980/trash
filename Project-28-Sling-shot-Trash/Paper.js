class Paper {
    constructor(x, y, radius) {
      var options = {
        'restitution':0.2,
          isStatic: false,
          'friction':0.5,
          'density':1.2
      }
      this.x = x;
      this.y = y;
      this.radius = radius;
      this.body = Bodies.circle(x, y, radius, options);
     
      this.image = loadImage("paper.png");
      //this.image.scale = 1.8;

      World.add(world, this.body);
    }
    display(){
      var pos =this.body.position;
      push();
      translate(pos.x, pos.y);
      //ellipseMode(CENTER);
      //fill("red");
      imageMode(CENTER);
      image(this.image, 0, 0, this.radius, this.radius);
      pop();
    }
  };

