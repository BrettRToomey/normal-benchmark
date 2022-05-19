results <- read.csv(file="report/Irwin Hall (12 draws) AVX.csv")

N <- min(1e5, length(results$value))

values <- sample(results$value, N)

y <- rnorm(N)
qqplot(values, y, main="Irwin Hall (12 draws) AVX")
qqline(y, col = 2, lwd = 2, lty = 2)

results <- read.csv(file="report/Intel SVML.csv")

values <- sample(results$value, N)

qqplot(values, y, main="Intel SVML")
qqline(y, col = 2, lwd = 2, lty = 2)

