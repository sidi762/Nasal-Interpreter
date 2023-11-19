use std.process_bar;

var ppm = func(filename, width, height, RGB) {
    # P3 use ASCII number
    # P6 use binary character
    var fd = io.open(filename, "wb");
    io.write(fd, "P6\n"~width~" "~height~"\n255\n");
    for(var i = 0; i<height; i += 1) {
        for(var j = 0; j<width; j += 1) {
            io.write(fd, RGB(i, j));
        }
    }
    io.close(fd);
}

var width = 1920;
var height = 1080;
var init_x = -0.75;
var init_y = 0.3;
var bar = (os.platform()=="windows")?
    process_bar.bar(front:"sharp", back:"point", sep:"line", length:50):
    process_bar.high_resolution_bar(50);
var f = func(i, j) {
    var (yMin, yMax, xMin, xMax) = (-1.35, 1.35, -2.4, 2.4);
    var (yDel, xDel) = (yMax-yMin, xMax-xMin);
    var (y, x) = (init_y, init_x);
    var (y0, x0) = ((i/height)*yDel+yMin, (j/width)*xDel+xMin);
    for(var iter = 0; iter<64; iter += 1) {
        var (x1, y1) = ((x0*x0)-(y0*y0)+x, 2*x0*y0+y);
        (x0, y0) = (x1, y1);
        if ((x0*x0)+(y0*y0)>4) {
            break;
        }
    }
    var progress = (i*width+j+1)/(width*height);
    if (progress*100-int(progress*100)==0) {
        print(bar.bar(progress), " ", progress*100, "%          \r");
    }
    iter = iter>=25? 255:int(iter/25*255);
    var c = char(iter);
    return c~c~c;
}

ppm("juliaset.ppm", width, height, f);
println();
