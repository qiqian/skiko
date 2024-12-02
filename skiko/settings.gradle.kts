pluginManagement {
    repositories {
        maven("https://maven.qq-home.com:8443/repository/maven-central/")
        maven("https://maven.qq-home.com:8443/repository/gradle-plugin-portal/")
    }
    buildscript {
        repositories {
            maven("https://maven.qq-home.com:8443/repository/maven-central/")
            maven("https://maven.qq-home.com:8443/repository/jetbrains-compose-internal/")
            maven("https://maven.qq-home.com:8443/repository/jetbrains-space/")
        }
        dependencies {
            classpath("org.jetbrains.compose.internal.build-helpers:publishing:0.1.3")
            classpath("org.kohsuke:github-api:1.116")
        }
    }

    plugins {
        val kotlinVersion = extra["kotlin.version"] as String
        kotlin("jvm").version(kotlinVersion)
        kotlin("multiplatform").version(kotlinVersion)
    }
}
rootProject.name = "skiko"
include("ci")
include("import-generator")
