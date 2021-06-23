class Dustbin {
    constructor(x, y, width, height) {
      var options = {
          'restitution':0.5,
          isStatic: true,
          'friction': 1.0,
          'density':1.5
      }
      this.body = Bodies.rectangle(x, y, width, height, options);
      this.width = width;
      this.height = height;
      this.image = loadImage("dustbin.png");
      
  
      World.add(world, this.body);
    }
    display(){
      var pos =this.body.position;
      push();
      translate(this.body.position.x, this.body.position.y);
      imageMode(CENTER)
      //imageMode(CENTER)
      //rectMode(CENTER)
      //this.image.scale = 0.02;
      //fill(255);
      angleMode(RADIANS);
      image(this.image, 0,-this.height/2,this.width, this.height)
      //rect(pos.x, .y, this.width, this.height);
      
      pop();
  
    }
  };
  
  