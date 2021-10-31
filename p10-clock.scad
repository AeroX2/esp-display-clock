$fn = 10;

p10_width = 363;
p10_height = 187;
p10_depth = 22;

border_size = 5;

difference() {
    box();
    translate([0,0,border_size]) internal();
}

module box() {
    difference() {
        minkowski() {
            translate([-border_size,-border_size,-border_size])
            cube([p10_width+border_size*2,p10_height+border_size*2,p10_depth+border_size*2]);
            sphere(10);
        }
    }
}

module internal() {
    for (i = [0: 32]) {
        for (ii = [0: 16]) {
            translate([i*11,ii*11,p10_depth]) cube(10);
        }
    }
    cube([p10_width,p10_height,p10_depth]);
}
        