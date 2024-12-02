val kspVersion: String by project

plugins {
    kotlin("multiplatform")
}

repositories {
    maven("https://maven.qq-home.com:8443/repository/maven-central/")
}

kotlin {
    jvm()
    sourceSets {
        val jvmMain by getting {
            dependencies {
                compileOnly(kotlin("compiler-embeddable"))
            }
        }
    }
}