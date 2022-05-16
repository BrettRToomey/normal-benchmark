results <- read.csv(file="results.csv")
results <- results[order(results$cycles,decreasing = TRUE),]
print(results)

algorithms <- results$algorithm
cycles <- results$cycles
dev <- results$stddev

par(mar=c(5.1, 8, 4.1, 2.1))
plot <- barplot(cycles, names=algorithms, cex.names=0.7, xlim=c(0, 65), xlab = "Cycles per sample", col="#c8a2c8", horiz=TRUE, las=1)
arrows(results$max, plot, results$min, plot, angle=90, code=3, length=0.04)
text(results$max+0.5, plot, labels=as.character(cycles), cex=0.7, adj = c(0, NA))