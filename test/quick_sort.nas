import.std.sort;

var var_sort = sort.var_sort;
var sort = sort.sort;


var vec=[];
setsize(vec, 1e4);
rand(time(0));
for(var i=0;i<1e4;i+=1)
    vec[i] = int(rand()*1e5);
sort(vec);
for(var i=1;i<1e4;i+=1) {
    if (vec[i]<vec[i-1]) {
        die("incorrect sort result");
    }
}

var test=func(n){
    var a=[];
    setsize(a,n);
    for(var i=0;i<n;i+=1){
        a[i]=int(rand()*n);
    }
    var ts=maketimestamp();
    ts.stamp();
    var_sort(a);
    println("[time] ",str(n)," in ",ts.elapsedMSec()/1000," sec (direct cmp)");

    var a=[];
    setsize(a,n);
    for(var i=0;i<n;i+=1){
        a[i]=int(rand()*n);
    }
    ts.stamp();
    sort(a);
    println("[time] ",str(n)," in ",ts.elapsedMSec()/1000," sec (lambda cmp)");
}

for(var i=1000;i<1e6;i*=10){
    test(i);
}