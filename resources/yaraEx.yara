rule ExampleRule
{
    strings:
        $my_text_string = "testtesttesttesttesttesttesttesttesttest"

    condition:
        $my_text_string
}
rule Test
{
    strings:
        $my_text_string = "jsapod;lsadjf;lsakjdf;ljsad;lfkjsald;kfjsaldkfjksahfdkjsahfd"

    condition:
        $my_text_string
}