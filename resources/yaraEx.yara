rule ExampleRule
{
    strings:
        $my_text_string = "text here"

    condition:
        $my_text_string
}
rule Test
{
    strings:
        $my_text_string = "test"

    condition:
        $my_text_string
}