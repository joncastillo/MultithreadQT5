// https://forums.adobe.com/message/7246695  
// Get the histogram data  
// ----------------------  
var luminosity = activeDocument.histogram;  
var red = activeDocument.channels["Red"].histogram;  
var green = activeDocument.channels["Green"].histogram;  
var blue = activeDocument.channels["Blue"].histogram;  
  
  
var datFile = new File("~/Desktop/Histogram.csv");  
  
  
datFile.open("e");  
datFile.writeln("Level,Luminosity,Red,Green,Blue\r");  
for ( i = 0; i <= 255; i++ ) {  
  datFile.writeln(i + "," + luminosity[i] + "," + red[i] + "," + green[i] + "," + blue[i] + "\r");  
}  
datFile.close();  