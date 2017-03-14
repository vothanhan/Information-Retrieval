mydata=read.table("D:\\Academic Files\\CS\\CS419\\Assignment 5\\PR\\talk.politics.misc.txt")
colnames(mydata)<-c("Recall","Precision")
plot(mydata[,"Recall"],mydata[,"Precision"],"l")

