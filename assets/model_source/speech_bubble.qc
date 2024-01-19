$modelname "speech_bubble.mdl"

$cd "."
$cdtexture "."
$cliptotextures
$externaltextures

$body "body" "speech_bubble"

$sequence "idle" {
    "anims/idle"
    loop
}
$sequence "talking" {
    "anims/talking"
    loop

}

$scale 32.0