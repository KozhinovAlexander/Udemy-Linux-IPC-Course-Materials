# Udemy-Linux-IPC-Course-Materials
Udemy Linux IPC Course Materials contains my code for the project from [Linux Inter Process Communication (IPC) from Scratch in C](https://www.udemy.com/course/linuxipc/?utm_source=adwords&utm_medium=udemyads&utm_campaign=Search_DSA_Beta_Prof_la.EN_cc.ROW-English&campaigntype=Search&portfolio=ROW-English&language=EN&product=Course&test=&audience=DSA&topic=&priority=Beta&utm_content=deal4584&utm_term=_._ag_162511579564_._ad_696197165427_._kw__._de_c_._dm__._pl__._ti_dsa-1677053911888_._li_9042300_._pd__._&matchtype=&gad_source=1&gad_campaignid=21168154305&gbraid=0AAAAADROdO2_QfoKD2OJZ5qiXwHLzgEjT&gclid=CjwKCAjw4efDBhATEiwAaDBpblHXQNjYjISpsi9IPdmWXNtquceZNyOSspgK1ssQcdF2rRmxyGC9PRoCXKcQAvD_BwE&couponCode=PMNVD2525) Udemy course.

## Setup

1. **Install dependencies**
   Run the provided setup script:
   ```sh
   ./setup.sh
   ```

## Build

You can use the provided `build.sh` script or build manually with CMake:

### Using the build script
```sh
./build.sh
```

### Manual build with CMake
```sh
mkdir -p build
cd build
cmake ..
make
```

The built binaries will be located in the `build/` directory.
