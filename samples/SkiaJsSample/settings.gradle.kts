pluginManagement {
    repositories {
        mavenLocal()
        maven("https://maven.qq-home.com:8443/repository/maven-central/")
        maven("https://maven.qq-home.com:8443/repository/gradle-plugin-portal/")
        maven {
            url = uri("https://dl.bintray.com/kotlin/kotlin-eap")
        }
    }
    plugins {
        val kotlinVersion = extra["kotlin.version"] as String
        kotlin("multiplatform").version(kotlinVersion)
    }

}
rootProject.name = "SkiaJsSample"

if (extra.properties.getOrDefault("skiko.composite.build", "") == "1") {
    includeBuild("../../skiko") {
        dependencySubstitution {
            substitute(module("org.jetbrains.skiko:skiko")).using(project(":"))
        }
    }
}
