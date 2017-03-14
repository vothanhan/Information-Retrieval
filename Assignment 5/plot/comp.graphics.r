mydata=read.table("D:\\Academic Files\\CS\\CS419\\Assignment 5\\PR\\comp.graphics.txt")
colnames(mydata)<-c("Recall","Precision")
plot(mydata[,"Recall"],mydata[,"Precision"],"l")

