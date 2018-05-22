### periodic.json ###

This file is used for the micro portion of multiscale.
It follows the following format:
```c++
{
    //"pairs" is an array of arrays, each containing pairs of numbers representing each element's geometry type and its ID
    "pairs": [ 
	[ (pair1), (pair2), ...],   //array1 of pairs
	[ ... ],                    //array2 of pairs
        ...
    ]  //end of "pairs"
}
```

Each `pair` has the following format:	
```c++   
// type: 1=vertex, 2=curve, 3=surface, 4=region, 5=patch, 7=interface
//  the type can be a "string" or an int
{ "type": ("string" OR int), "ID": (int) }