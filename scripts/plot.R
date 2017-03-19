library(ggplot2)

## Returns the number of reachable nodes with an x-tile abstraction of the 12-puzzle
reachable <- function(x) {
    stopifnot(is.numeric(x))

    y <- 12

    c <- factorial(y) / 2
    
    if(x < 2)
        c
    else
       (c*2) / factorial(x)
}


predictionsource <- "3x4_predictions"
predstatssource <- "3x4_pred_stats"
abstractsstatssource <- "3x4_abstraction_stats"

data <- read.csv(abstractsstatssource,sep=",",header=T)


aggdata <- aggregate(cbind(expanded = data$expanded,
                           duplicated = data$duplicated,
                           generated = data$generated),
                     by=list(g=data$g,
                             level=data$level,
                             predseed=data$predseed),
                     FUN=sum, na.rm=TRUE)

aggdata$totalexpanded <- ave(aggdata$expanded, aggdata$level, aggdata$predseed, FUN=cumsum)
aggdata$proportionexplored <- aggdata$totalexpanded / sapply(aggdata$level, FUN = reachable)


aggdata$dupprob <- aggdata$duplicated / aggdata$generated




actual <- subset(aggdata, level == 0)
sample <- subset(aggdata, level >= 0 & level < 11)


result <- aggregate(cbind(dupprob = sample$dupprob,
                          proportionexplored = sample$proportionexplored),
                    by=list(level=sample$level, g=sample$g),
                    FUN = function(x) c(mean = mean(x), sd = sd(x)))

png("g-level_vs_duplicates_plot.png", width = 1920, height=1080, res=220)

p <- ggplot(data=result,
              aes(g, dupprob[,"mean"],
                  colour = factor(level),
                  shape = factor(level))) +
    geom_line() +
    geom_point() +
    scale_shape_manual(values = seq(0,16),
                       guide = guide_legend("Abstraction Level")) +
    scale_colour_discrete(guide = guide_legend("Abstraction Level")) +
    ggtitle("The Effect of Abstraction Level on the Duplicate Probability Distribution") +
    xlab("g-level") +
    ylab("Mean Duplicate Probability")


print(p)

dev.off()

png("g-level_vs_explored_plot.png", width = 1920, height=1080, res=220)

p <- ggplot(data=result,
              aes(g, proportionexplored[,"mean"],
                  colour = factor(level),
                  shape = factor(level))) +
    geom_line() +
    geom_point() +
    scale_shape_manual(values = seq(0,16),
                       guide = guide_legend("Abstraction Level")) +
    scale_colour_discrete(guide = guide_legend("Abstraction Level")) +
    ggtitle("The Effect of Abstraction Level on the Proportion of the Space Explored") +
    xlab("g-level") +
    ylab("Mean Explored Proportion")


print(p)

dev.off()

png("explored_vs_duplicates_plot.png", width = 1920, height=1080, res=220)

p <- ggplot(data=result,
              aes(proportionexplored[,"mean"], dupprob[,"mean"],
                  colour = factor(level),
                  shape = factor(level))) +
    geom_point() +
    geom_line() +
    scale_shape_manual(values = seq(0,16),
                       guide = guide_legend("Abstraction Level")) +
    scale_colour_discrete(guide = guide_legend("Abstraction Level")) +
    scale_x_log10() +
    #scale_y_log10() +
    ggtitle("Comparison of Explored Proportions and Duplicate Probabilities") +
    xlab("Mean Explored Proportion") +
    ylab("Mean Duplicate Probability")


print(p)

dev.off()

aggdata <- aggregate(cbind(expanded = aggdata$expanded,
                           duplicated = aggdata$duplicated,
                           generated = aggdata$generated),
                     by=list(level=aggdata$level,
                             predseed=aggdata$predseed),
                     FUN=sum, na.rm=TRUE)

actual <- subset(aggdata, level == 0)
sample <- subset(aggdata, level > 0 & level < 11)

sample$searchcompression <- sample$expanded / actual$expanded[1]
sample$reachablecompression <- sapply(sample$level, FUN = function(x)(100 * reachable(x) / reachable(0)))
sample$proportionexplored <- sample$expanded / sapply(sample$level, FUN = reachable)

## For each level of abstractions, get the set of all trials at that level of abstraction
result <- aggregate(cbind(searchcompression = sample$searchcompression,
                          expanded = sample$expanded,
                          reachablecompression = sample$reachablecompression,
                          proportionexplored = sample$proportionexplored),
                    by=list(level=sample$level),
                    FUN = function(x) c(mean = mean(x), sd = sd(x)))

## Our prediction curves are ouptutted to "results1.png"
png("search_tree_compression_plot.png", width = 1920, height=1080, res=220)

p <- ggplot(data=result) +
    geom_point(aes(x=level, y=searchcompression[,"mean"])) +
    geom_errorbar(mapping=aes(x=level,
                              ymin = searchcompression[,"mean"] - searchcompression[,"sd"],
                              ymax = searchcompression[,"mean"] + searchcompression[,"sd"]),
                 width = 0.2) +
#    geom_point(aes(x=level, y=proportionexplored[,"mean"], colour="#FF000")) +
#    geom_point(aes(x=level, y=reachablecompression[,"mean"], colour="#000FF")) +
    scale_x_continuous(breaks=seq(0,25)) +
    #scale_y_log10() +
    ggtitle("The Effect of Abstraction Level on Search Tree Compression") +
    xlab("Abstraction Level") +
    ylab("Mean Search Tree Compression (+- Std. Dev)")


print(p)

dev.off()


aggdata <- aggregate(cbind(g = data$g),
                     by=list(level=data$level,
                             predseed=data$predseed),
                     FUN=max, na.rm=TRUE)

actual <- subset(aggdata, level == 0)
sample <- subset(aggdata, level > 0 & level < 11)

sample$compression <- sample$g / actual$g[1]

result <- aggregate(cbind(compression = sample$compression),
                    by=list(level=sample$level),
                    FUN = function(x) c(mean = mean(x), sd = sd(x)))

png("path_compression_plot.png", width = 1920, height=1080, res=220)

p <- ggplot(data=result) +
    geom_point(aes(level, compression[,"mean"])) +
    geom_errorbar(mapping=aes(x=level,
                              ymin = compression[,"mean"] - compression[,"sd"],
                              ymax = compression[,"mean"] + compression[,"sd"]),
                  width = 0.2) +
    scale_x_continuous(breaks=seq(0,25)) +
    ggtitle("The Effect of Abstraction Level on Solution Path Compression") +
    xlab("Abstraction Level") +
    ylab("Mean Solution Path Compression (+- Std. Dev)")


print(p)

dev.off()



data <- read.csv(predstatssource,sep=",",header=T)

aggdata <- aggregate(cbind(expanded = data$expanded,
                           duplicated = data$duplicated,
                           generated = data$generated),
                     by=list(g=data$g,
                             level=data$level,
                             predseed=data$predseed),
                     FUN=sum, na.rm=TRUE)

aggdata$dupprob <- aggdata$duplicated / aggdata$generated

actual <- subset(aggdata, level == 0)
sample <- subset(aggdata, level >= 0 & level < 11)

result <- aggregate(cbind(dupprob = sample$dupprob),
                    by=list(level=sample$level, g=sample$g),
                    FUN = function(x) c(mean = mean(x), sd = sd(x)))



png("pred_stats_plot.png", width = 1920, height=1080, res=220)

p <- ggplot(data=result,
              aes(g, dupprob[,"mean"],
                  colour = factor(level),
                  shape = factor(level))) +
    geom_line() +
    geom_point() +
    scale_shape_manual(values = seq(0,16),
                       guide = guide_legend("Abstraction Level")) +
    scale_colour_discrete(guide = guide_legend("Abstraction Level")) +
    ggtitle("Duplicate Probability Distribution After Adjusting for Solution Path Compression") +
    xlab("g-level") +
    ylab("Mean Duplicate Probability")


print(p)

dev.off()




## Read experiment results
data <- read.csv(predictionsource,sep=",",header=T)

actual <- subset(data, level == 0)
sample <- subset(data, level > 0 & level < 11)
sample$percentage <- sample$expanded / actual$expanded[1]

## For each level of abstractions, get the set of all trials at that level of abstraction
result <- aggregate(cbind(percentage = sample$percentage),
                    by=list(level=sample$level),
                    FUN = function(x) c(mean = mean(x), sd = sd(x)))

## Our prediction curves are ouptutted to "results1.png"
png("predictions_plot.png", width = 1920, height=1080, res=220)

p <- ggplot(data=result) +
    geom_point(aes(level, percentage[,"mean"])) +
    geom_errorbar(mapping=aes(x=level,
                              ymin = percentage[,"mean"] - percentage[,"sd"],
                              ymax = percentage[,"mean"] + percentage[,"sd"]),
                  width = 0.2) +
    scale_x_continuous(breaks=seq(0,25)) +
    ggtitle("Predicted No. Expanded Nodes as a Proportion of the Actual No. Nodes Expanded") +
    xlab("Abstraction Level") +
    ylab("Mean Proportion of Actual (+- Std. Dev)")


print(p)

dev.off()
