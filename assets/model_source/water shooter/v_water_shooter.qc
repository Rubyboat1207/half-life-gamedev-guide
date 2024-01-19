$modelname "v_water_shooter.mdl"

$cd "."
$cdtexture "."
$cliptotextures
$externaltextures

$body "body" "WaterShooter"

$sequence "idle" {
    "anims/idle"
    loop
}

$sequence "shoot" "anims/shoot"
$sequence "reload" "anims/reload"

$scale 32.0