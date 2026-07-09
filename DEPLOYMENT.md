# Thru Alphanet Deployment Notes

## Counter Program

Program account:
taUrjDxskuPtXAsXWgVWqHy-yc37G7JmV8m9uDiPMvEn5B

Counter account:
tasGhF_XtO_s-H_9imDSXm5ON8iFeSIQpJ94h0xaDIwRov

Status:
- Counter program compiled
- Counter program deployed
- Counter account created
- Increment transaction executed successfully
- Counter event emitted successfully

## Guestbook V1

Program account:
taBD7C5jeKOFKP85IXLXai5hyK6bhzrYJ9m8eohJK3akt9

Status:
- Guestbook V1 compiled
- Guestbook V1 deployed
- Post message transaction executed successfully
- Event type: 1
- Message event output works correctly

Example message:
gm thru from indonesia

---

## Guestbook V2

Guestbook V2 upgrades the guestbook from event-only to stateful.

Program account:

\taXqFPQKYei0k7J5lq5GoMuw2VBydfmA7DuewfCUxYsGrD
\
State account:

\tawMEal1YExM-Io7nmaWve1-XSo2R7P54fjXSO6K7hqq4F
\
Status:

- Guestbook V2 compiled
- Guestbook V2 deployed as a new program
- Guestbook state account created
- Post message transaction executed successfully
- Account state updated with latest message
- Event emitted with event_type = 2

Example message:

\gm thru v2 from indonesia
\EOF

---

## Guestbook V2

Guestbook V2 upgrades the guestbook from event-only to stateful.

Program account:
taXqFPQKYei0k7J5lq5GoMuw2VBydfmA7DuewfCUxYsGrD

State account:
tawMEal1YExM-Io7nmaWve1-XSo2R7P54fjXSO6K7hqq4F

Status:
- Guestbook V2 compiled
- Guestbook V2 deployed as a new program
- Guestbook state account created
- Post message transaction executed successfully
- Account state updated with latest message
- Event emitted with event_type = 2

Example message:
gm thru v2 from indonesia
