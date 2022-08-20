import.stl.process_bar;

var ppm=func(filename,width,height,RGB){
    # P3 use ASCII number
    # P6 use binary character
    var fd=io.open(filename,"wb");
    io.write(fd,"P3\n"~width~" "~height~"\n255\n");
    for(var i=0;i<height;i+=1){
        for(var j=0;j<width;j+=1)
            io.write(fd,RGB(i,j));
        io.write(fd,"\n");
    }
    io.close(fd);
}

var width=1280;
var height=720;
var bar=process_bar.bar(front:os.platform()=="windows"?"sharp":"block",back:"point",sep:"line",length:50);
var f=func(i,j){
    var (yMin,yMax,xMin,xMax)=(-0.9,0.9,-2.2,1);
    var (yDel,xDel)=(yMax-yMin,xMax-xMin);
    var (y,x)=((i/height)*yDel+yMin,(j/width)*xDel+xMin);
    var (x0,y0)=(x,y);
    for(var iter=0;iter<25;iter+=1){
        var (x1,y1)=((x0*x0)-(y0*y0)+x,2*x0*y0+y);
        (x0,y0)=(x1,y1);
        if((x0*x0)+(y0*y0)>4){
            break;
        }
    }
    var progress=(i*width+j+1)/(width*height);
    print(bar.bar(progress)," ",progress*100,"%          \r");
    iter=int(iter/25*255);
    return iter~" "~iter~" "~iter~" ";
}
ppm("a.ppm",width,height,f);
println("\nfinished.");
