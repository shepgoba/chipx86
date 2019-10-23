_main:
    LD V0, 0H
    LD V1, 0H
    LD I, 222H
    RND V2, 01H
    SE V2, 01H
    LD I, 21EH
    DRW V0, V1, 4H
    ADD V0, 04H
    SE V0, 40H
    JP 204H
    LD V0, 0H
    ADD V1, 04H
    SE V1, 20H
    JP 204H
    JP 21CH
    LD V0, V4
    CALL 10H
    CALL 40H
    LD V0, V1
